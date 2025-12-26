# Chapter 7: Memory Management

## Introduction

Memory management is the foundation upon which AGENTS builds. Every term, every structure, every and-box lives in memory, and that memory must be allocated efficiently, organized coherently, and eventually reclaimed. This chapter explains how AGENTS organizes memory into distinct spaces, allocates objects with bump-pointer allocation, handles alignment requirements across platforms, and manages the heap's block-based structure.

**Note**: This chapter focuses on **allocation and organization**. Garbage collection—the reclamation of unused memory—is covered separately in Chapter 8.

## Memory Spaces

AGENTS divides memory into three distinct spaces, each with different characteristics and lifetimes:

### 1. Heap Space (Dynamic Allocation)

**Purpose**: Storage for runtime-allocated terms and structures

**Contains**:
- List cells `[H|T]`
- Structures `f(...)`
- Unbound variables
- Generic objects (floats, bignums, ports)
- And-boxes and choice-boxes
- Execution states
- Suspension lists

**Characteristics**:
- **Dynamically allocated** during execution
- **Garbage collected** when space runs low
- **Block-based organization** (explained below)
- **Grows on demand** by allocating new blocks

**Access**: Via global pointer `glob_heapcurrent`

From `storage.h:16-19`:

```c
#define heapcurrent   glob_heapcurrent
#define heapend       glob_heapend

extern heap  *glob_heapcurrent;   // Current allocation pointer
extern heap  *glob_heapend;       // End of current block
```

### 2. Constant Space (Immortal Objects)

**Purpose**: Storage for constants that live for the program's lifetime

**Contains**:
- Atoms (`foo`, `bar`, `[]`)
- Functors (`point/2`, `append/3`)
- Predicate structures
- Built-in definitions

**Characteristics**:
- **Never garbage collected** (immortal)
- **Allocated once** at creation
- **Shared globally** (same atom everywhere is same pointer)
- **Hash-table organized** for fast lookup

**Access**: Via global pointers `constspace`, `constcurrent`, `constend`

From `storage.c:29-31`:

```c
heap  *constspace;       // Start of constant space
heap  *constcurrent;     // Current allocation point
heap  *constend;         // End of constant space
```

**Why separate?**: Atoms and functors are **permanent**. Mixing them with garbage-collectable heap would waste GC time scanning them repeatedly. By keeping them separate, GC can ignore constant space entirely.

### 3. Stack Spaces (Various)

**Purpose**: Stack-like structures for execution control

**Includes**:
- **Trail**: Records variable bindings for backtracking
- **Task queue**: Pending agents to execute
- **Recall stack**: Saved execution states
- **Wake queue**: Suspended agents waiting to wake
- **Context stack**: Environment frames

**Characteristics**:
- **Stack discipline** (push/pop)
- **Bounded sizes** (configured at startup)
- **Overflow triggers GC** or error
- **Not garbage collected** (explicitly managed)

These are part of the execution state (Chapter 5) and grow/shrink with computation depth.

## Block-Based Heap Organization

### The Block Structure

The heap is organized as a **linked list of blocks**. Each block is a contiguous region of memory:

From `storage.h:322-330`:

```c
typedef char heap;    // Heap is byte-addressable

typedef struct block {
    flipstate  flipflop;      // FLIP or FLOP (for GC)
    struct block *next;       // Next block in list
    heap  *start;             // Start address
    heap  *end;               // End address
    heap  *logical_end;       // Allocation boundary
} block;
```

**Fields:**

- **flipflop**: Which GC space this block belongs to (Chapter 8)
- **next**: Pointer to next block (NULL if last)
- **start**: Beginning of usable memory
- **end**: End of block (one past last byte)
- **logical_end**: Where allocations stop (may be < end)

### Block Lists

AGENTS maintains several block lists:

From `storage.c:19-25`:

```c
block  *global;          // Current active heap
block  *current_block;   // Block we're allocating in
block  *unused;          // Free blocks (recycled)
block  *used;            // Blocks from previous GC space
block  *consth;          // Constant space block
```

**Block lifecycle:**

```
┌─────────────┐
│  Allocated  │ (malloc)
└──────┬──────┘
       │
       ▼
┌─────────────┐
│   unused    │ (free list)
└──────┬──────┘
       │ heap_block()
       ▼
┌─────────────┐
│   global    │ (active heap)
│ (current_   │
│  _block)    │
└──────┬──────┘
       │ GC
       ▼
┌─────────────┐
│    used     │ (old space)
└──────┬──────┘
       │ gc_end()
       ▼
┌─────────────┐
│   unused    │ (recycled)
└─────────────┘
```

**Advantages of blocks:**

1. **Incremental growth**: Allocate new blocks as needed without copying entire heap
2. **Bounded allocation**: Each malloc is fixed-size (configurable `blocksize`)
3. **Recycling**: Old blocks are reused, avoiding repeated malloc/free
4. **Limit control**: Threshold for GC based on number of blocks

### Block Allocation

From `storage.c:65-81`:

```c
block *heap_block()
{
    block *free;

    if (unused == NULL) {
        // No recycled blocks, allocate new
        free = balloc(blocksize);
        if (free == NULL) {
            FatalError("couldn't allocate heap space");
        }
    } else {
        // Reuse recycled block
        free = unused;
        unused = unused->next;
        free->next = NULL;
    }

    free->logical_end = free->end;
    free->flipflop = currentflip;   // Mark for GC
    return free;
}
```

**Process:**
1. Check `unused` list for recycled blocks
2. If empty, call `balloc()` to malloc a new block
3. If found, reuse from `unused` list
4. Mark block with current GC space (FLIP or FLOP)
5. Return for use

## Bump-Pointer Allocation

AGENTS uses the classic **bump-pointer** (linear) allocation strategy:

```
Heap block:
┌────────────────────────────────────────────────────┐
│ │█│█│█│█│█│█│█│█│█│█│                              │
│ ▲                  ▲                               ▲
│ start          heapcurrent                       end
└────────────────────────────────────────────────────┘

█ = Allocated
│ = Free
```

**Allocation**: Simply increment `heapcurrent` by the size needed.

### Allocation Macros

From `storage.h:36-47`:

```c
#define NEW(ret, type) \
{ \
    (ret) = (type *) heapcurrent; \
    heapcurrent += sizeof(type); \
    if (heapcurrent >= heapend) { \
        SaveHeapRegisters(); \
        reinit_heap((ret), sizeof(type)); \
        RestoreHeapRegisters(); \
        (ret) = (type *) heapcurrent; \
        heapcurrent += sizeof(type); \
    } \
}
```

**Process:**
1. Point `ret` to current `heapcurrent`
2. Increment `heapcurrent` by size of type
3. If we've exceeded `heapend`, allocate a new block
4. Retry the allocation in the new block

**Advantages:**
- **Fast**: Just a pointer increment (a few instructions)
- **No fragmentation**: Contiguous allocation
- **Cache-friendly**: Sequential access pattern

**Disadvantages:**
- **No reuse**: Can't reclaim individual objects (need GC)
- **Block overflow**: Must handle crossing block boundaries

### Overflow Handling

When `heapcurrent >= heapend`, we've filled the current block:

From `storage.c:107-125`:

```c
void reinit_heap(ret, size_needed)
    heap *ret;
    long size_needed;
{
    if (size_needed > blocksize) {
        char buf[256];
        sprintf(buf, "Cannot create object larger than %ld bytes",
                blocksize);
        FatalError(buf);
    }

    if (active >= limit) {
        gcflag = ON;    // Trigger GC
    }

    active++;
    current_block->logical_end = ret;
    current_block->next = heap_block();   // Get new block
    current_block = current_block->next;
    heapcurrent = current_block->start;
    heapend = current_block->end;
}
```

**Process:**
1. Check if object is too large (exceeds block size)
2. Check if we've exceeded block limit (trigger GC)
3. Set `logical_end` to mark how much was used
4. Allocate new block via `heap_block()`
5. Link it to the chain
6. Reset `heapcurrent` and `heapend` to new block

**Note**: GC doesn't run immediately—it sets `gcflag = ON`. The next `CheckExceptions` in the engine (Chapter 9) actually runs GC.

### Specialized Allocation Macros

AGENTS provides type-specific allocation macros:

**NewReference** (allocate a reference cell):
```c
#define NewReference(ret) \
{ \
    (ret) = (Reference) heapcurrent; \
    heapcurrent += RefSize; \
    if (heapcurrent >= heapend) { \
        /* handle overflow */ \
    } \
}
```

**NewList** (allocate a list cell):
```c
#define NewList(ret) \
{ \
    (ret) = (list *) (heapcurrent + ENV_OFFSET); \
    heapcurrent += LstSize; \
    if (heapcurrent >= heapend) { \
        /* handle overflow */ \
    } \
}
```

**NewStructure** (allocate a structure with N arguments):
```c
#define NewStructure(ret, arity) \
{ \
    (ret) = (structure *) (heapcurrent + ENV_OFFSET); \
    heapcurrent += StrSize(arity); \
    if (heapcurrent >= heapend) { \
        /* handle overflow */ \
    } \
}
```

**NewFloat** (allocate a float with proper alignment):
```c
#define NewFloat(ret) \
{ \
    // Align to sizeof(double)
    if ((uword)heapcurrent % sizeof(double) != 0) { \
        heapcurrent += sizeof(double) -
                       (uword)heapcurrent % sizeof(double); \
    } \
    (ret) = (Fp) heapcurrent; \
    heapcurrent += FltSize; \
    if (heapcurrent >= heapend) { \
        /* handle overflow */ \
    } \
}
```

These macros optimize for specific types, avoiding function call overhead.

## Alignment Requirements

### Platform Differences

**Alignment** refers to where in memory objects can be placed. Some architectures require strict alignment:

**x86-64** (relaxed):
- Can access unaligned data
- Performance penalty for unaligned access
- Prefers alignment but tolerates misalignment

**ARM64** (strict):
- **Requires** aligned access for most operations
- Unaligned access may **fault** (crash)
- Must enforce alignment

**Alpha** (strict):
- Similar to ARM64
- Unaligned access not allowed

### WORDALIGNMENT Macro

From `term.h:66-70`:

```c
#ifndef TADBITS
#define TADBITS 32
#endif

#define WORDALIGNMENT (TADBITS/8)   // 4 on 32-bit, 8 on 64-bit
```

**WORDALIGNMENT** is the minimum alignment for pointers:
- **32-bit**: 4 bytes
- **64-bit**: 8 bytes

### WordAlign Macro

From `storage.h:5`:

```c
#define WordAlign(x) \
    (((x) + (WORDALIGNMENT-1)) & ~((uword)(WORDALIGNMENT-1)))
```

**Purpose**: Round up `x` to the next multiple of WORDALIGNMENT.

**Example** (64-bit, WORDALIGNMENT = 8):
```
WordAlign(0)  = 0
WordAlign(1)  = 8
WordAlign(7)  = 8
WordAlign(8)  = 8
WordAlign(9)  = 16
WordAlign(15) = 16
```

**How it works:**
1. Add `WORDALIGNMENT - 1` to `x`
2. Mask off low bits with `~((uword)(WORDALIGNMENT-1))`

For WORDALIGNMENT = 8:
- Mask = `~7` = `0xFFFFFFFFFFFFFFF8`
- Clears low 3 bits, rounding to multiple of 8

### OptionalWordAlign

Some platforms need **stricter** alignment:

From `storage.h:7-9`:

```c
#ifndef OptionalWordAlign
#define OptionalWordAlign(x)  x
#endif
```

**Default**: No-op (just returns `x`)

**Macintosh** (from `sysdeps.h:28-30`):
```c
#ifdef macintosh
#define OptionalWordAlign(x)  WordAlign((unsigned long)(x))
#endif
```

**Effect**: Forces alignment even when not strictly required.

### Alignment in Allocation

**Lists and structures** must be aligned for tagging to work:

```c
#define NewList(ret) \
{ \
    (ret) = (list *) (heapcurrent + ENV_OFFSET); \
    heapcurrent += LstSize; \
    /* ... */ \
}
```

**ENV_OFFSET**: Ensures alignment by adjusting the base pointer if needed.

**Why critical**: The low bits must be zero for tags (Chapter 6). If a pointer is misaligned, the tag bits would be corrupted.

### Floating-Point Alignment

**Floats** (doubles) need special alignment:

From `storage.h:89-103`:

```c
#define NewFloat(ret) \
{ \
    if ((uword)heapcurrent % sizeof(double) != 0) { \
        heapcurrent += sizeof(double) -
                       (uword)heapcurrent % sizeof(double); \
    } \
    (ret) = (Fp) heapcurrent; \
    heapcurrent += FltSize; \
    /* ... */ \
}
```

**Why**: On many platforms, unaligned `double` access causes:
- **Performance penalty** (x86-64)
- **Bus error** (SPARC, ARM64)
- **Undefined behavior** (C standard)

By aligning to `sizeof(double)` (usually 8), we avoid these issues.

## Constant Space Management

### Initialization

From `storage.c:198-209`:

```c
void init_constspace(size)
    unsigned int size;
{
    constsize = size;
    constspace = (heap *)malloc(size);
    if (constspace == NULL) {
        FatalError("couldn't allocate constant space");
    }
    constcurrent = constspace;
    constend = constspace + size;
}
```

**Process:**
1. Allocate a single large block for constants
2. Set `constcurrent` to start
3. Set `constend` to mark boundary

### Constant Allocation

**Atoms**:

```c
atom *new_atom(char *name) {
    atom *a = (atom *)constcurrent;
    constcurrent += sizeof(atom);

    if (constcurrent >= constend) {
        FatalError("constant space exhausted");
    }

    a->name = strdup(name);   // Name stored separately
    a->number = next_atom_id++;
    return a;
}
```

**Functors**: Similar, allocate in constant space.

**Key point**: No recycling. Once allocated, constants live forever.

## PTR_ORG: Pointer Origin Adjustment

### The Problem

On some platforms, pointers have **high bits set** that would conflict with tags or GC mark bits.

Example: HP-PA architecture uses address space starting at `0x40000000`. If we tried to tag these pointers directly, the high bits would corrupt our tags.

### The Solution: PTR_ORG

From `sysdeps.h:17-20`:

```c
#ifdef hppa
#  define PTR_ORG  ((uword)0x40000000)
#endif
```

From `term.h:TagRef`:

```c
#define TagRef(p)   ((Term)((uword)(p) - PTR_ORG))
```

**Process:**
1. **Before tagging**: Subtract PTR_ORG from pointer
2. **After untagging**: Add PTR_ORG back

**Effect**: Normalize pointers to a base of 0, avoiding high-bit conflicts.

### Modern Platforms

From `sysdeps.h`:

```c
#ifndef PTR_ORG
#  define PTR_ORG  ((uword)0)
#endif
```

**x86-64 and ARM64**: User space addresses start near 0. PTR_ORG = 0 (no adjustment needed).

## Heap State Management

### Saving Heap State

Before calling functions that might allocate:

```c
#define SaveHeap(B, H) \
{ \
    (H) = heapcurrent; \
    (B) = current_block; \
}
```

**Saves**:
- Current allocation pointer
- Current block

### Restoring Heap State

After function returns (if needed):

```c
#define RestoreHeap(B, H) \
{ \
    if (current_block != (B)) { \
        restore_heap_block(B); \
        heapend = current_block->end; \
    } \
    heapcurrent = (H); \
}
```

**Restores**:
- Allocation pointer
- Current block (if changed)
- Releases blocks allocated during the call

**Use case**: Backtracking. If a goal fails after allocating, we can restore heap to before the attempt, reclaiming space without GC.

### Heap Block Restoration

From `storage.c:128-144`:

```c
void restore_heap_block(saved_current)
    block *saved_current;
{
    block *last;

    current_block = saved_current;

    // Find the last block in new chain
    for (last = current_block->next;
         last->next != NULL;
         last = last->next) {
        active -= 1;
    }
    active -= 1;

    // Move unused blocks to free list
    last->next = unused;
    unused = current_block->next;
    current_block->logical_end = current_block->end;
    current_block->next = NULL;
}
```

**Effect**: Deallocate blocks allocated since save point, returning them to `unused` list.

## Memory Statistics

### Heap Size

From `storage.c:191-194`:

```c
int heap_size()
{
    return (blocksize * (active - 1)) +
           (heapcurrent - current_block->start);
}
```

**Calculation**:
- Full blocks: `blocksize * (active - 1)`
- Current block: `heapcurrent - current_block->start`

### Active Blocks

From `storage.c:86-105`:

```c
int active;    // Number of active blocks

void init_heap(size, lmt)
    unsigned int size, lmt;
{
    /* ... */
    active = 1;    // Start with one block
    /* ... */
}
```

**Tracking**: Incremented when allocating new blocks, decremented when recycling.

### GC Threshold

From `storage.c:84, 116-118`:

```c
long gcratio;    // GC threshold ratio

if (active >= limit) {
    gcflag = ON;
}

// After GC:
limit = (100 * active) / gcratio;
```

**Dynamic limit**: GC threshold adjusts based on heap usage after last GC.

If `gcratio = 80` and `active = 10` after GC:
- `limit = (100 * 10) / 80 = 12.5` → 12 blocks

Next GC triggers when `active >= 12`.

## Performance Characteristics

### Allocation Speed

**Bump-pointer**: Extremely fast
- **Best case**: 2-3 CPU cycles (pointer increment + comparison)
- **Overflow case**: ~1000 cycles (block allocation, linking)

**Constant allocation**: One-time cost, negligible amortized.

### Memory Overhead

**Per block**: `sizeof(block)` = ~40 bytes
- Negligible compared to `blocksize` (typically 256KB - 1MB)

**Alignment padding**:
- Worst case: `WORDALIGNMENT - 1` bytes per object (7 bytes on 64-bit)
- Typical: ~10-15% of heap (empirical)

**Fragmentation**: None (linear allocation, GC compacts)

### Cache Behavior

**Linear allocation**: Excellent spatial locality
- Objects allocated together are adjacent in memory
- Good for iterating through structures

**Block-based**: Some cache pollution when crossing blocks
- Blocks may not be adjacent in virtual memory
- Minor impact (modern CPUs prefetch well)

## Configuration

### Heap Size

Default from `Makefile`:

```makefile
AKLHEAPLIMIT = 1000      # Max blocks before GC
blocksize = 262144       # 256 KB per block
```

### Constant Space

Default:

```c
constsize = 1048576;     // 1 MB
```

### Tuning

**For large programs**: Increase `blocksize` to reduce block allocations.

**For small memory**: Decrease `blocksize` and `AKLHEAPLIMIT`.

**For many atoms**: Increase `constsize`.

## Summary

AGENTS memory management is **simple but effective**:

- **Three spaces**: Heap (garbage collected), constant (permanent), stacks (managed)
- **Block-based heap**: Linked list of fixed-size blocks, grows on demand
- **Bump-pointer allocation**: Fast linear allocation within blocks
- **Alignment**: Platform-specific (relaxed on x86-64, strict on ARM64)
- **PTR_ORG**: Pointer normalization for problematic platforms
- **Recycling**: Blocks reused across GC cycles
- **Dynamic limits**: GC threshold adapts to program behavior

This foundation enables the copying garbage collector (Chapter 8) to work efficiently, reclaiming unused memory while maintaining fast allocation.

The next chapter explains how garbage collection identifies live objects, copies them to a new space, and reclaims the old space—all without fragmenting memory or stopping the world for long periods.

---

**Next Chapter**: [Chapter 8: Garbage Collection](chapter-08-garbage-collection.md)
