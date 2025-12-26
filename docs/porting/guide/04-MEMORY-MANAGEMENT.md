# AGENTS v0.9 - Memory Management Deep Dive

**Document:** Phase 2, Document 4 of 10
**Status:** Complete
**Date:** 2025-11-04
**Purpose:** Detailed analysis of memory model, alignment, and garbage collection

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Memory Architecture](#memory-architecture)
3. [Heap Allocation](#heap-allocation)
4. [Alignment Requirements](#alignment-requirements)
5. [Garbage Collection Algorithm](#garbage-collection-algorithm)
6. [Tagged Pointer Mechanics](#tagged-pointer-mechanics)
7. [Constant Space Management](#constant-space-management)
8. [Copy Operations](#copy-operations)
9. [Porting Implications](#porting-implications)
10. [References](#references)

---

## Executive Summary

AGENTS uses a **two-space copying garbage collector** with sophisticated memory management:

### Memory Spaces

| Space | Purpose | GC | Size |
|-------|---------|----|----|---|
| **Heap** | Dynamic terms | ✅ Yes | Configurable blocks |
| **Constant Space** | Atoms, functors | ❌ Never | Fixed size |
| **Stacks** | Trail, tasks, etc. | ❌ Never | Grows dynamically |

### Key Characteristics

- **Block-based allocation:** Heap divided into linked blocks
- **Bump-pointer allocation:** Fast O(1) allocation
- **Copying GC:** Two-space with flip-flop
- **Forwarding pointers:** High bit marks forwarded objects
- **Conservative:** Scans all roots thoroughly

### Architecture Dependencies

⚠️ **Medium Risk:**
- Alignment requirements (WORDALIGNMENT)
- Tagged pointer manipulations
- High-bit mark usage (bit 31/63)
- PTR_ORG adjustments

✅ **Alpha proves 64-bit works**

---

## Memory Architecture

### Three-Space Model

From `emulator/storage.c:19-40`:

```c
block    *global;          // Current from-space
block    *current_block;   // Current allocation block
block    *unused;          // Free block pool
block    *used;            // To-space (during GC)

heap     *glob_heapend;       // End of current block
heap     *glob_heapcurrent;   // Allocation pointer

heap     *constspace;      // Constants start
heap     *constcurrent;    // Constants allocation pointer
heap     *constend;        // Constants end

gcvalue  gcflag;          // GC trigger flag (ON/OFF)
```

**Memory Layout:**

```
┌─────────────────────────────────────────────────────┐
│                  ADDRESS SPACE                       │
├─────────────────────────────────────────────────────┤
│  Constant Space (Never GC'd)                        │
│  ├── Atoms (double-word aligned)                    │
│  ├── Functors                                       │
│  └── Constant floats                                │
├─────────────────────────────────────────────────────┤
│  Heap Space (Copying GC)                            │
│  ├── Block 1 (FLIP or FLOP)                         │
│  │   ├── Terms, structures, lists                   │
│  │   ├── And-boxes, choice-boxes                    │
│  │   ├── Continuations                              │
│  │   └── Suspensions                                │
│  ├── Block 2 (same flipstate)                       │
│  ├── Block 3...                                     │
│  └── Block N                                        │
├─────────────────────────────────────────────────────┤
│  GC Stacks (malloc'd, grows as needed)              │
│  ├── Reference stack (gcrefstack)                   │
│  ├── Suspension stack (gcsuspstack)                 │
│  └── Generic stack (gnstack)                        │
├─────────────────────────────────────────────────────┤
│  Execution Stacks (in exstate)                      │
│  ├── Trail                                          │
│  ├── Task queue                                     │
│  ├── Wake queue                                     │
│  ├── Recall stack                                   │
│  └── Context stack                                  │
└─────────────────────────────────────────────────────┘
```

---

## Heap Allocation

### Block Structure

From `emulator/storage.h:334-340`:

```c
typedef struct block {
  flipstate   flipflop;      // FLIP or FLOP (for GC)
  struct block *next;        // Next block in chain
  heap        *start;        // Block start address
  heap        *end;          // Block end address
  heap        *logical_end;  // Actual allocation limit
} block;
```

**Key Fields:**
- `flipflop`: Which GC space (FLIP=0, FLOP=1)
- `start/end`: Physical block boundaries
- `logical_end`: Where allocation stopped (for backtracking)

### Allocation Strategy

From `emulator/storage.h:36-47`:

```c
#define NEW(ret, type) \
{ \
    (ret) = (type *) heapcurrent; \
    heapcurrent += sizeof(type); \
    if(heapcurrent >= heapend) {\
        SaveHeapRegisters(); \
        reinit_heap((ret),sizeof(type)); \
        RestoreHeapRegisters(); \
        (ret) = (type *) heapcurrent; \
        heapcurrent += sizeof(type); \
    }\
}
```

**Mechanism:**
1. **Bump pointer:** `heapcurrent` advances
2. **Fast path:** If space available, done
3. **Slow path:** Allocate new block if full
4. **GC trigger:** If too many blocks active

**Performance:** O(1) amortized, very fast

### Block Allocation

From `emulator/storage.c:46-81`:

```c
block *balloc(size)
     int size;
{
  block *new;
  char *seg;
  new = (block *)malloc(sizeof(block));
  seg = (char *)malloc(size);
  if(new != NULL && seg != NULL) {
    new->next = NULL;
    new->start = seg;
    new->end = seg + size;
    return new;
  } else {
    return NULL;
  }
}

block *heap_block()
{
  block *free;
  if(unused == NULL) {
    free = balloc(blocksize);  // Allocate new block
    if(free == NULL) {
      FatalError("couldn't allocate heap space");
    }
  } else {
    free = unused;             // Reuse from pool
    unused = unused->next;
    free->next = NULL;
  }
  free->logical_end = free->end;
  free->flipflop = currentflip;  // Mark with current space
  return free;
}
```

**Block Recycling:**
- Blocks freed by GC go to `unused` pool
- New allocations first try to reuse from pool
- Only call `malloc()` if pool empty

**Typical Block Size:** 200KB (AKLBLOCKSIZE from constants.h)

---

## Alignment Requirements

### Word Alignment

From `emulator/term.h:78-80`:

```c
#ifndef WORDALIGNMENT
#define WORDALIGNMENT  (TADBITS/8)
#endif
```

**32-bit:** WORDALIGNMENT = 4 bytes
**64-bit:** WORDALIGNMENT = 8 bytes

### Alignment Macro

From `emulator/storage.h:5-9`:

```c
#define WordAlign(x) (((x) + (WORDALIGNMENT-1)) & ~((uword)(WORDALIGNMENT-1)))

#ifndef OptionalWordAlign
#define OptionalWordAlign(x)  x  // No-op by default
#endif
```

**Usage:**
- Round up address to next aligned boundary
- Bit trick: `(x + 7) & ~7` for 8-byte alignment

### Platform-Specific Alignment

**Macintosh** (strict alignment required):
```c
#ifdef macintosh
#define OptionalWordAlign(x)  WordAlign((unsigned long)(x))
#endif
```

**ARM64 Consideration:**
- May need `OptionalWordAlign` like Macintosh
- Unaligned access can fault or be slow

### Float Alignment

From `emulator/storage.h:89-103`:

```c
#define NewFloat(ret)\
{\
    if((uword)heapcurrent % sizeof(double) != 0) {\
       heapcurrent += sizeof(double) - (uword)heapcurrent % sizeof(double);\
    }\
    (ret) = (Fp) heapcurrent; \
    heapcurrent += FltSize; \
    if(heapcurrent >= heapend) {\
        SaveHeapRegisters(); \
        reinit_heap((ret),SMALL_SIZE); \
        RestoreHeapRegisters(); \
        (ret) = (Fp) heapcurrent; \
        heapcurrent += FltSize; \
    }\
}
```

**Special handling:**
- Floats must be `sizeof(double)` aligned (8 bytes)
- Check alignment before allocation
- Add padding if misaligned

**Reason:** Floating-point hardware requires natural alignment

### Atom Alignment

From `emulator/storage.h:250-260`:

```c
#define NewAtom(atm)\
{ \
    if(((uword)constcurrent & PTR_ATM_BIT) != 0) {\
       constcurrent += PTR_ATM_BIT;\
    }\
    (atm) = (atom*)constcurrent;\
    constcurrent += AtmSize;\
    if(constcurrent > constend){\
      FatalError("out of constant space");\
    }\
}
```

**Special constraint:**
- Atoms must NOT have `PTR_ATM_BIT` set (bit 4)
- If address has bit 4 set, skip forward
- Ensures atom addresses are distinguishable from small ints

**From term.h:83:**
```c
#define IMM_ATM_BIT  ((uword)0x10)  // Bit 4
```

---

## Garbage Collection Algorithm

### Two-Space Copying GC

**Algorithm:** Cheney's algorithm (breadth-first copying)

**Spaces:**
- **From-space:** Current heap (being scanned)
- **To-space:** New heap (being allocated)
- **Flip-flop:** Alternate between FLIP and FLOP states

### GC Trigger

From `emulator/storage.c:107-125`:

```c
void reinit_heap(ret, size_needed)
   heap *ret;
   long size_needed;
{
  if (size_needed > blocksize) {
    char buf[256];
    sprintf(buf, "Cannot create object larger than %ld bytes", blocksize);
    FatalError(buf);
  }
  if(active >= limit) {
    gcflag = ON;  // Trigger GC
  }
  active++;
  current_block->logical_end = ret;
  current_block->next = heap_block();
  current_block = current_block->next;
  heapcurrent = current_block->start;
  heapend = current_block->end;
}
```

**Trigger condition:**
```c
if(active >= limit)
```

Where:
- `active`: Number of currently allocated blocks
- `limit`: Dynamic limit based on GC ratio
- `limit = (100 * active_after_gc) / gcratio`
- Default `gcratio = 25` (trigger when 4x growth)

### GC Entry Point

From `emulator/gc.c:175-200`:

```c
void gc(exs)
     exstate *exs;
{
  exstate *super, *active, *tmp;

  // Begin GC: allocate to-space
  gc_begin();

  // Copy live objects
  gc_exstate(exs);  // Start from execution state roots

  // End GC: reclaim from-space
  gc_end();

  // Reset flag
  gcflag = OFF;
}
```

### GC Phases

**1. gc_begin() - Setup**

From `emulator/storage.c:147-162`:

```c
void gc_begin()
{
  used = global;  // Save from-space

  // Flip spaces
  currentflip = (currentflip == FLIP ? FLOP : FLIP);

  // Allocate first to-space block
  global = heap_block();
  active = 1;
  current_block = global;
  heapcurrent = current_block->start;
  heapend = current_block->end;
  gcflag = OFF;
}
```

**2. Copying Phase**

**Root Set:**
- Execution state registers (areg, yreg)
- And-boxes and choice-boxes
- Trail entries
- Task/wake/recall queues
- Constraint variables
- Suspension lists

**Copying Strategy:**
- Depth-first for structures/lists
- Breadth-first for references (using stack)
- Forwarding pointers to handle sharing

**3. gc_end() - Cleanup**

From `emulator/storage.c:164-184`:

```c
void gc_end()
{
  block *last = used;
  for(;;) {
#ifdef DEBUG
    heap *ptr = last->start;
    // Clear old heap (debug mode)
    while (ptr != last->end) {
      *ptr++ = 0;
    }
#endif
    if (last->next == NULL)
      break;
    last = last->next;
  }
  // Return from-space blocks to unused pool
  last->next = unused;
  unused = used;
  used = NULL;

  // Adjust limit based on survival rate
  limit = (100 * active) / gcratio;
}
```

### Forwarding Pointers

From `emulator/term.h:205-220`:

```c
#define ForeWord(A)       (*(tagged_address*)(A))
#define ForwOf(FW)        ((void*)((FW)-MarkBitMask))
#define IsForwMarked(FW)  (((tagged_address)(FW)) & MarkBitMask)

#define IsCopied(X)       IsForwMarked(ForeWord(X))

#define Link(Old,New) \
    (*(tagged_address*)(Old)) = (tagged_address)(New) + MarkBitMask

#define Forw(Old)         ForwOf(ForeWord(Old))
```

**Mechanism:**
1. When object copied, first word overwritten with new address
2. High bit set to mark as forwarding pointer
3. Subsequent references follow forwarding pointer
4. Enables copying of circular structures

**MarkBitMask:**
```c
#define MarkBitMask  ((uword)0x1 << (TADBITS-1))
```

- 32-bit: `0x80000000` (bit 31)
- 64-bit: `0x8000000000000000` (bit 63)

**Why high bit?**
- User-space addresses typically < 0x80000000 (32-bit)
- User-space addresses typically < 0x00007fffffffffff (64-bit)
- High bit available for marking

---

## Tagged Pointer Mechanics

### Tag Layout (32-bit)

```
Bit:  31 30...........6 5 4 3 2 1 0
      |M|   Address    |   Tag    |

M = Mark bit (for GC forwarding)
Address = 26 bits (word-aligned, 2 low bits available)
Tag = 2-bit primary tag (bits 1-0)
      4-bit secondary tag (bits 3-0) for some types
      6-bit tag for immediates (bits 5-0)
```

### Tag Layout (64-bit)

```
Bit:  63 62................6 5 4 3 2 1 0
      |M|    Address        |   Tag    |

M = Mark bit (for GC forwarding)
Address = 58 bits (word-aligned, 3 low bits available)
Tag = Same as 32-bit
```

### Pointer Downshifting

Many operations require "downshifting" pointers by 2 bits:

From `emulator/term.h:158-164`:

```c
#define Oth(X)        ((X) >> 2)
#define Tho(X)        ((X) << 2)
#define OthTad(X)     Oth(Tad(X))
#define OthTagOffset(T) (Oth(TagOffset(T)) - PTR_ORG)
#define PTR_ATM_BIT   Oth(IMM_ATM_BIT)
#define AddressOth(X) (AddressOf(Oth(X)) + PTR_ORG)
```

**Why?**
- 4-bit tags require pointer >> 2
- After shift, add back PTR_ORG if needed

### PTR_ORG Adjustments

From `emulator/term.h:87-95`:

```c
#ifndef PTR_ORG
#define PTR_ORG 0
#endif

#define TadOth(X, T)  (Tho((tagged_address)(X))-Tho(PTR_ORG)+TagOffset(T))
#define AddressOth(X) (AddressOf(Oth(X)) + PTR_ORG)
```

**Purpose:**
- Some platforms map user space at high addresses
- HP-PA: PTR_ORG = 0x40000000
- AIX: PTR_ORG = 0x20000000
- Subtract PTR_ORG before tagging
- Add back when dereferencing

**Modern platforms (x86-64, ARM64):**
- PTR_ORG = 0 (like Alpha)
- User space starts at 0x00000000

---

## Constant Space Management

### No Garbage Collection

From `emulator/storage.c:198-210`:

```c
void init_constspace(size)
    unsigned int size;
{
  char *seg;

  constsize = size;

  seg = (char *)malloc(size);

  if(seg != NULL) {
    constspace = seg;
    constcurrent = seg;
    constend = seg + size;
  } else {
    FatalError("couldn't allocate constant space");
  }
}
```

**Characteristics:**
- Single `malloc()` at startup
- Never freed
- Never garbage collected
- Atoms and functors live forever

**Size:** 400KB default (AKLCONSTSIZE from constants.h)

### Allocation Macros

**Atoms:**
```c
#define NewAtom(atm)\
{ \
    if(((uword)constcurrent & PTR_ATM_BIT) != 0) {\
       constcurrent += PTR_ATM_BIT;\
    }\
    (atm) = (atom*)constcurrent;\
    constcurrent += AtmSize;\
    if(constcurrent > constend){\
      FatalError("out of constant space");\
    }\
}
```

**Functors:**
```c
#define NewFunctor(fnk)\
{ \
    (fnk) = (functor*)constcurrent;\
    constcurrent += FnkSize;\
    ((Generic)((fnk)))->method = &functormethod; \
    if(constcurrent > constend){\
      FatalError("out of constant space");\
    }\
}
```

**No overflow handling:** Fatal error if constant space exhausted

---

## Copy Operations

### Non-Deterministic Promotion

**File:** `emulator/copy.c` (~800 lines)

**Purpose:** Copy terms when promoting and-boxes (for backtracking)

**Different from GC:**
- Selective copying (only non-local terms)
- Must be **reversible** (for backtracking)
- Uses similar forwarding pointer technique

### Copy vs GC

| Aspect | Copy | GC |
|--------|------|--------|
| **Trigger** | And-box promotion | Heap full |
| **Scope** | Selective (non-local) | Everything |
| **Reversible** | Yes (for backtracking) | No |
| **Forwarding** | Yes | Yes |
| **Stacks** | Separate (copyrefstack) | Separate (gcrefstack) |

---

## Porting Implications

### 32-bit → 64-bit Changes

**Automatic:**
- ✅ TADBITS = 64 → WORDALIGNMENT = 8
- ✅ All structures double in size (pointers)
- ✅ Small integers get 58 bits (vs 26)
- ✅ High bit still available for marking (bit 63)

**Requires Testing:**
- ⚠️ Alignment on ARM64 (may need OptionalWordAlign)
- ⚠️ PTR_ORG = 0 (verify no conflicts)
- ⚠️ Float alignment (should be fine)
- ⚠️ Atom alignment (PTR_ATM_BIT check)

### Alpha Port Lessons

**What Alpha proved:**
- ✅ MarkBitMask in bit 63 works
- ✅ PTR_ORG = 0 works
- ✅ 8-byte alignment works
- ✅ Forwarding pointers work
- ✅ No redesign needed

### x86-64 Expectations

**Advantages:**
- Little-endian (matches Alpha)
- Relaxed alignment (unaligned access OK but slower)
- Large address space (48-bit virtual)
- PTR_ORG = 0 should work

**Potential Issues:**
- None expected (very similar to Alpha)

### ARM64 Expectations

**Advantages:**
- Little-endian (typical)
- Large address space
- Register-rich (good for HARDREGS)

**Potential Issues:**
- ⚠️ **Strict alignment preferred**
  - May fault on unaligned access
  - Solution: Enable `OptionalWordAlign`
- ⚠️ **Memory ordering**
  - Weaker memory model than x86-64
  - May need memory barriers (unlikely to affect single-threaded code)

**Test Strategy:**
1. Run with OptionalWordAlign disabled first
2. If SIGBUS errors, enable OptionalWordAlign
3. Verify all allocations properly aligned

---

## Testing Strategy

### Unit Tests

**Test 1: Alignment**
```c
void test_alignment() {
  heap *base = heap_start();
  assert((uword)base % WORDALIGNMENT == 0);

  NewReference(ref);
  assert((uword)ref % WORDALIGNMENT == 0);

  NewList(lst);
  assert((uword)lst % WORDALIGNMENT == 0);

  NewFloat(flt);
  assert((uword)flt % sizeof(double) == 0);
}
```

**Test 2: Tag Integrity**
```c
void test_tags() {
  Term t = TagNum(42);
  assert(IsNUM(t));
  assert(GetSmall(t) == 42);

  Term ref = TagRef(some_addr);
  assert(IsREF(ref));
  assert(Ref(ref) == some_addr);
}
```

**Test 3: GC Forwarding**
```c
void test_forwarding() {
  Term *old = alloc_term();
  Term *new = alloc_term();

  Link(old, new);
  assert(IsCopied(old));
  assert(Forw(old) == new);
}
```

**Test 4: PTR_ORG**
```c
void test_ptr_org() {
  heap *addr = 0x12345678;
  Term t = TagLst(addr);
  List l = Lst(t);
  assert(l == addr);  // Should round-trip correctly
}
```

### Integration Tests

**GC Stress Test:**
1. Allocate many objects
2. Trigger multiple GCs
3. Verify all objects still accessible
4. Check no memory leaks

**Alignment Stress:**
1. Allocate mixed objects (lists, floats, atoms)
2. Verify all properly aligned
3. On ARM64: Should not SIGBUS

---

## References

### Source Files Analyzed

**Memory Management:**
- `emulator/storage.c:1-210` - Heap and constant space
- `emulator/storage.h:1-379` - Allocation macros
- `emulator/gc.c:1-200+` - Garbage collector
- `emulator/gc.h` - GC interface
- `emulator/copy.c` - Copy operations

**Tagged Pointers:**
- `emulator/term.h:1-738` - Complete tagging scheme

**Platform Dependencies:**
- `emulator/sysdeps.h:1-42` - PTR_ORG, alignment

### Related Documents

- `01-ARCHITECTURE-OVERVIEW.md` - System overview
- `02-EMULATOR-ANALYSIS.md` - Emulator internals
- `03-PLATFORM-DEPENDENCIES.md` - Platform ports
- `07-TESTING-STRATEGY.md` - Testing approach

---

## Revision History

| Date | Version | Changes |
|------|---------|---------|
| 2025-11-04 | 1.0 | Complete memory management analysis |

---

**End of Document**
