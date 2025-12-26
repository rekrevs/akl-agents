# Chapter 8: Garbage Collection

## Introduction

Memory is finite. As AGENTS executes, it allocates terms, structures, and-boxes, choice-boxes—thousands or millions of objects. Eventually, the heap fills up. **Garbage collection** (GC) reclaims memory occupied by objects that are no longer reachable, making space for new allocations.

AGENTS uses a **two-space copying collector**, a classic and elegant algorithm that trades space for simplicity and performance. This chapter explains how it works, why it was chosen, and how it integrates with the execution engine.

**Related chapters**: Chapter 7 (Memory Management) explains allocation; this chapter explains reclamation.

## The Two-Space Copying Algorithm

### Basic Concept

The heap is divided into **two semispaces**:

```
┌──────────────────────┐
│   FROM space (FLIP)  │  Objects currently allocated
│   ─────────────────  │
│   [active objects]   │
│   [garbage]          │
└──────────────────────┘

┌──────────────────────┐
│   TO space (FLOP)    │  Empty (or previously used)
│   ─────────────────  │
│   [empty]            │
└──────────────────────┘
```

**Allocation**: In FROM space only.

**When heap fills**: GC triggers.

**GC process:**
1. Identify **roots** (entry points to live objects)
2. **Copy** live objects from FROM to TO
3. Update all **pointers** to point to new locations
4. **Swap** FROM and TO
5. FROM space (now empty) becomes the new TO

**Result**: All live objects compacted in TO. All garbage left behind in FROM (which is now discarded).

### Flip and Flop

AGENTS calls the two spaces **FLIP** and **FLOP**:

From `storage.h:327-337`:

```c
typedef enum {
    FLIP = 0,
    FLOP = 1
} flipstate;

typedef struct block {
    flipstate  flipflop;      // Which space this block belongs to
    /* ... */
} block;

extern flipstate currentflip;  // Current allocation space
```

**At any time**, one space is FROM (current), the other is TO (target).

**After GC**: Spaces swap roles.

```
Before GC:
  currentflip = FLIP (allocating in FLIP blocks)

GC runs, copies from FLIP to FLOP

After GC:
  currentflip = FLOP (now allocating in FLOP blocks)

Next GC:
  Copy from FLOP to FLIP
  currentflip = FLIP again
```

This **flip-flop** continues indefinitely.

## GC Triggering

### When GC Triggers

From `storage.c:116-118`:

```c
if (active >= limit) {
    gcflag = ON;
}
```

**Trigger condition**: Number of active blocks exceeds limit.

**Note**: GC doesn't run immediately. It sets `gcflag = ON`. The execution engine checks this flag and calls GC when safe.

From `engine.c:93-102`:

```c
#define CheckExceptions \
{ \
    if (gcflag) { \
        exs->arity = arity; \
        exs->andb = andb; \
        exs->insert = insert; \
        gc(exs);    // ← GC runs here
        andb = exs->andb; \
        insert = exs->insert; \
        /* ... */ \
    } \
}
```

**Why delayed?**: Engine must be in a **consistent state** (execution state saved, registers synced).

### Dynamic Limit

After GC, the limit adjusts:

From `storage.c:183`:

```c
limit = (100 * active) / gcratio;
```

**Example**: If `active = 10` blocks after GC and `gcratio = 80`:
- `limit = (100 * 10) / 80 = 12.5` → 12 blocks

**Effect**: Next GC triggers when heap grows by ~25% (from 10 to 12 blocks).

**Rationale**: Adapt to program's memory usage. Programs that retain more data will have higher limits.

## GC Algorithm

### Phase 1: Initialization

From `storage.c:147-162`:

```c
void gc_begin()
{
    used = global;    // Move current heap to "used" list

    // Flip spaces
    currentflip = (currentflip == FLIP ? FLOP : FLIP);

    // Allocate fresh TO space
    global = heap_block();
    active = 1;
    current_block = global;

    heapcurrent = current_block->start;
    heapend = current_block->end;
    gcflag = OFF;
}
```

**Process:**
1. Save current heap (FROM space) in `used` list
2. Flip `currentflip` (FLIP ↔ FLOP)
3. Allocate new empty blocks for TO space
4. Reset allocation pointer to start of TO space

### Phase 2: Root Scanning

**Roots** are pointers to heap objects that are directly accessible:

From `gc.c:175-240` (simplified):

```c
void gc(exstate *exs)
{
    gcstate gcst;
    InitState(gcst);

    // 1. Scan execution state
    gc_exstate(exs, &gcst);

    // 2. Scan constant space references (if any)
    gc_constants(&gcst);

    // 3. Scan trail
    gc_trail(exs, &gcst);

    // 4. Scan and-box/choice-box tree
    gc_tree(exs, &gcst);

    // 5. Scan any global roots
    gc_global_roots(&gcst);

    // Done
}
```

**Roots include:**

**Execution state**:
- Argument registers (`areg[0..MAX_AREGS]`)
- Y registers (environment variables)
- Program counter (if pointing to heap)
- And-box and choice-box structures

**And-box tree**:
- All active and-boxes
- Their continuations
- Their wake lists

**Choice-box tree**:
- All choice points
- Saved argument registers
- Trail entries

**Trail**:
- Bindings that may be undone on backtracking

### Phase 3: Copying (Cheney's Algorithm)

AGENTS uses **Cheney's algorithm** for copying:

```
TO space:
┌─────────────────────────────────────────────┐
│ [copied][copied][copied] │               │
│  ▲                       ▲               │
│  scan                   alloc            │
└─────────────────────────────────────────────┘
```

**Two pointers:**
- **scan**: Next object to process
- **alloc**: Next free location (same as `heapcurrent`)

**Algorithm:**

```
1. Copy roots to TO space
2. scan = start of TO space
3. while (scan < alloc):
     object = *scan
     for each pointer P in object:
         if P points to FROM space:
             copy *P to TO space
             update P to new location
     scan++
4. Done when scan == alloc (no more objects to process)
```

This is **breadth-first traversal** of the object graph.

### Phase 4: Forwarding Pointers

**Problem**: When we copy an object, other references to it must be updated. How do we know where an object moved to?

**Solution**: **Forwarding pointers**

When an object is copied:

```c
Term gc_term(Term t)
{
    Term *addr = AddressOf(t);

    if (IsMarked(*addr)) {
        // Already copied, forwarding pointer exists
        return GetForwardingPointer(*addr);
    }

    // Copy to TO space
    Term *new_addr = heapcurrent;
    Copy(addr, new_addr, size_of(t));
    heapcurrent += size_of(t);

    // Install forwarding pointer
    *addr = MakeForwardingPointer(new_addr);

    return TagSameAs(new_addr, t);
}
```

**Forwarding pointer**: Stored in the **first word** of the old object, marked with the high bit.

From `term.h`:

```c
#define IsMarked(t)      ((t) & GC_MARK_BIT)
#define Mark(t)          ((t) | GC_MARK_BIT)
#define Unmark(t)        ((t) & ~GC_MARK_BIT)
#define Forw(t)          Unmark(t)
```

**GC_MARK_BIT**: The high bit (bit 63 on 64-bit, bit 31 on 32-bit).

**How it works:**
1. Copy object to TO space
2. Overwrite first word of FROM object with pointer to TO object **| GC_MARK_BIT**
3. When we encounter same FROM object again, check mark bit
4. If marked, read forwarding pointer (clear mark bit, dereference)

**Example:**

```
FROM space:
┌───────────────┐
│ List cell     │
│ [H | T]       │
└───────────────┘
  ▲
  │ addr = 0x1000

After copying:

FROM space:
┌───────────────┐
│ 0x2000 | MARK │  ← Forwarding pointer
└───────────────┘

TO space:
┌───────────────┐
│ List cell     │ ← 0x2000
│ [H' | T']     │
└───────────────┘
```

### Phase 5: Scanning Objects

For each object type, GC knows what pointers it contains:

**List cell**:
```c
void gc_list(list *l, gcstate *gcst)
{
    l->car = gc_term(l->car);    // Copy head
    l->cdr = gc_term(l->cdr);    // Copy tail
}
```

**Structure**:
```c
void gc_structure(structure *s, gcstate *gcst)
{
    functor *f = s->functor;    // Functor is in const space, don't copy
    for (int i = 0; i < f->arity; i++) {
        s->args[i] = gc_term(s->args[i]);
    }
}
```

**And-box**:
```c
void gc_andbox(andbox *ab, gcstate *gcst)
{
    ab->father = gc_choicebox(ab->father);
    ab->next = gc_andbox(ab->next);
    ab->cont = gc_continuation(ab->cont);
    gc_wake_list(ab->wake);
    // ... more fields
}
```

Each object type has a custom GC function that knows its structure.

### Phase 6: Finalization

From `storage.c:164-184`:

```c
void gc_end()
{
    block *last = used;

    // Move FROM blocks to unused list
    for (;;) {
        if (last->next == NULL)
            break;
        last = last->next;
    }
    last->next = unused;
    unused = used;
    used = NULL;

    // Adjust GC threshold
    limit = (100 * active) / gcratio;
}
```

**Process:**
1. FROM space blocks (in `used`) are recycled to `unused` list
2. They're now empty and ready for next GC cycle
3. Adjust GC threshold based on new heap size

## The Close List

### Generic Objects with External Resources

Some objects (ports, foreign objects) own **external resources** (file descriptors, malloc'd memory, etc.) that must be finalized when the object dies.

**Problem**: How do we know when such objects are no longer reachable?

**Solution**: The **close list**.

### Close List Mechanism

From `gc.c` (conceptual):

```c
typedef struct close_entry {
    Generic object;
    void (*finalizer)(Generic);
    struct close_entry *next;
} close_entry;

close_entry *close_list = NULL;

void register_for_close(Generic obj, void (*finalizer)(Generic))
{
    close_entry *entry = malloc(sizeof(close_entry));
    entry->object = obj;
    entry->finalizer = finalizer;
    entry->next = close_list;
    close_list = entry;
}
```

**During GC:**

```c
void gc_close()
{
    close_entry *entry = close_list;
    close_entry *new_list = NULL;

    while (entry != NULL) {
        if (!IsMarked(entry->object)) {
            // Object not copied, it's garbage
            entry->finalizer(entry->object);   // Finalize
            free(entry);                       // Remove from list
        } else {
            // Object still live, keep in list
            entry->object = Forw(entry->object);  // Update to new address
            entry->next = new_list;
            new_list = entry;
        }
        entry = entry->next;
    }

    close_list = new_list;
}
```

**Effect**: Dead objects have their finalizers called. Live objects' pointers are updated.

**Use cases**:
- **Ports**: Close stream when port dies
- **Foreign objects**: Free C-allocated memory
- **File handles**: Close file descriptors

## Performance Characteristics

### Time Complexity

**GC time**: Proportional to **live data size**, not total heap size.

**Formula**: `T_gc = k * live_objects`

Where `k` is the cost per object (copy + scan).

**Key insight**: Garbage is **free** to collect (we don't touch it).

**Best case**: Program with lots of garbage, little live data → fast GC.

**Worst case**: Program with mostly live data → slow GC (but rare GC).

### Space Complexity

**Space overhead**: **2× heap size**

We need **two semispaces** of equal size. At any time, one is empty.

**Trade-off**: We waste 50% of memory for simplicity and speed.

**Justification**: Memory is cheap, complexity is expensive. Two-space is the simplest collector that compacts.

### Pause Time

**Stop-the-world**: GC pauses all execution.

**Pause duration**: 10-100ms for typical programs.

**Factors**:
- Number of live objects
- Depth of object graph
- Heap fragmentation (none, we compact)

**Acceptable for AGENTS**: Most applications tolerate occasional 10-100ms pauses.

**Not suitable for**: Hard real-time systems requiring <1ms latencies.

### Allocation Speed

**Post-GC**: Allocation is **extremely fast** (bump-pointer).

**Compaction**: All live objects are contiguous, no fragmentation.

**Cache-friendly**: Sequential allocation and access patterns.

## Comparison with Other GC Algorithms

### vs. Mark-Sweep

**Mark-Sweep** (used in many Lisps, older Prologs):
1. **Mark phase**: Set mark bit on live objects
2. **Sweep phase**: Walk entire heap, free unmarked objects

**Advantages of Mark-Sweep**:
- Only one heap space needed (no 2× overhead)
- In-place (no copying)

**Disadvantages of Mark-Sweep**:
- **Fragmentation**: Freed objects leave holes
- **Slower allocation**: Need free list, not bump-pointer
- **Touches all memory**: Sweep phase walks entire heap

**Two-space advantages**:
- **No fragmentation**: Compacts automatically
- **Fast allocation**: Bump-pointer
- **Fast GC**: Only touches live objects

**Two-space disadvantages**:
- **2× space overhead**

**AGENTS' choice**: Two-space. Memory is cheaper than CPU time, and compaction is valuable.

### vs. Generational GC

**Generational GC** (used in Java, JavaScript VMs):
- Partition objects by age
- Collect young objects frequently (most garbage is young)
- Collect old objects rarely

**Advantages of Generational**:
- **Very fast minor GCs**: Only scan young generation
- **Lower pause times**: Smaller working set per GC

**Disadvantages of Generational**:
- **More complex**: Multiple generations, remembered sets, write barriers
- **Not always beneficial**: If object lifetimes are uniform

**AGENTS' choice**: Simple two-space. AKL programs don't have strong generational behavior (most objects are intermediate-lived, not very short or very long).

### vs. Reference Counting

**Reference Counting**:
- Each object has a count of references to it
- Increment on new reference, decrement on lost reference
- Free when count reaches zero

**Advantages**:
- **Immediate reclamation**: No GC pauses
- **Deterministic**: Finalization happens promptly

**Disadvantages**:
- **Cycles leak**: Cannot collect circular structures
- **High overhead**: Every pointer update increments/decrements counts
- **Cache pollution**: Scattered updates

**AGENTS' choice**: Two-space. AKL programs create cyclic structures (e.g., rational trees), which reference counting cannot handle.

## Integration with Execution

### Safe Points

GC can only run at **safe points** where:
- All roots are known
- Execution state is consistent
- No registers are in inconsistent states

**In AGENTS**: `CheckExceptions` after instructions that might allocate.

```c
CaseLabel(CALL):
    /* ... execute instruction ... */
    CheckExceptions;   // ← GC can run here
    NextOp();
```

**Why safe**: Execution state is in memory, registers are synced, no partial operations in progress.

### Root Enumeration

GC must know **all roots**. In AGENTS:

**Explicit roots**:
- Execution state: `exs->areg`, `exs->andb`, etc.
- And-box tree: Traverse from root
- Choice-box tree: Traverse from root
- Trail: All trailed bindings

**No hidden roots**: C local variables don't hold heap pointers (only tagged Terms, which are copied when GC runs).

### Pointer Updates

After copying, **all pointers** must be updated:

```c
// Before GC:
Term x = some_heap_object;

// During GC:
gc_term(x);   // Copies object, returns new pointer

// After GC:
x = gc_term(x);   // Update x to new location
```

**Critical**: Miss one pointer update → dangling pointer → crash.

**AGENTS' discipline**: All heap pointers go through `gc_term()` or equivalent.

## Edge Cases and Subtleties

### Self-Referential Structures

Unbound variables are **self-referential**:

```c
Term var = TagUva(&var);   // Points to itself
```

**During GC**:
1. Copy variable to TO space
2. Install forwarding pointer in FROM
3. When scanning TO copy, update self-reference to point to TO location

**Handled by**: Special case in `gc_term()` for unbound variables.

### Shared Substructures

Multiple pointers to the same object:

```
  X
 / \
Y   Z
 \ /
  W   ← Shared
```

**First time**: Copy W to TO, install forwarding pointer.
**Second time**: Find forwarding pointer, reuse TO copy.

**Result**: Sharing preserved.

### Cyclic Structures

Rational trees (cycles):

```
X = f(X)
```

**During GC**:
1. Copy `f(_)` to TO, mark FROM
2. Recurse to copy argument
3. Find marked FROM object → forwarding pointer
4. Update argument to TO location

**Cheney's algorithm** handles cycles naturally (breadth-first traversal detects already-copied objects).

### Trail Entries

The trail records variable bindings for backtracking. It contains **pointers to variables**, not copies.

**During GC**: Update trail entries to point to new locations:

```c
void gc_trail(exstate *exs, gcstate *gcst)
{
    for (trail_entry *e = exs->trail.start; e != exs->trail.top; e++) {
        e->var = (Term *)gc_term((Term)e->var);
    }
}
```

## GC Statistics and Monitoring

### GC Frequency

**Typical**: Every 10-100 seconds in long-running programs.

**Factors**:
- Allocation rate
- Object lifetimes
- Heap size
- GC threshold (`gcratio`)

### Heap Size After GC

**Key metric**: How much live data remains?

If `live_data / heap_size > 0.8`:
- Most of heap is live
- GC doesn't reclaim much
- Increase heap size or optimize program

If `live_data / heap_size < 0.2`:
- Lots of garbage
- GC is effective
- Current heap size is fine

### GC Debugging

With `-DGC_DEBUG`:

```c
#ifdef GC_DEBUG
insize = heap_size();
fprintf(stderr, "{GC, initial size %dK }\n", insize/1024);
#endif

/* ... GC runs ... */

#ifdef GC_DEBUG
outsize = heap_size();
fprintf(stderr, "{GC, final size %dK }\n", outsize/1024);
fprintf(stderr, "{GC, reclaimed %dK }\n", (insize-outsize)/1024);
#endif
```

**Output**:
```
{GC, initial size 1024K }
{GC, final size 256K }
{GC, reclaimed 768K }
```

## Future Optimizations

Potential GC improvements:

### 1. Incremental GC

Instead of collecting entire heap at once, collect in **small increments**.

**Advantage**: Lower pause times (1-10ms instead of 10-100ms).

**Disadvantage**: More complexity (read/write barriers, snapshots).

### 2. Generational GC

Partition heap by object age, collect young objects more frequently.

**Advantage**: Faster minor GCs (if most garbage is young).

**Disadvantage**: Complex implementation, may not fit AKL's allocation patterns.

### 3. Parallel GC

Use multiple threads to scan and copy in parallel.

**Advantage**: Faster GC on multicore CPUs.

**Disadvantage**: Synchronization overhead, more complex.

### 4. Concurrent GC

Run GC concurrently with program execution.

**Advantage**: No pauses (GC runs in background).

**Disadvantage**: Very complex (read/write barriers, consistency).

**Current status**: AGENTS uses simple two-space stop-the-world. It works well for the target applications. Advanced GC would add complexity without clear benefit.

## Summary

AGENTS' garbage collection is **simple and effective**:

- **Two-space copying**: FROM and TO semispaces, flip-flop between them
- **Cheney's algorithm**: Breadth-first traversal with forwarding pointers
- **Compacting**: All live objects moved to TO, no fragmentation
- **Proportional to live data**: Garbage is free to collect
- **Stop-the-world**: Pauses execution during GC (10-100ms)
- **Bump-pointer allocation**: Fast allocation after GC
- **Close list**: Finalizers for objects with external resources
- **2× space overhead**: Trade-off for simplicity

This collector has served AGENTS well for decades. It's easy to understand, easy to debug, and performs adequately for the system's use cases.

The next chapters explore other aspects of the implementation: unification, backtracking, constraints, and more.

---

**Next Chapter**: [Chapter 11: Unification](chapter-11-unification.md)
