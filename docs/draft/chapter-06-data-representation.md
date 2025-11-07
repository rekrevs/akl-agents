# Chapter 6: Data Representation

## Introduction

At the lowest level, AGENTS must represent all AKL data—atoms, numbers, structures, lists, variables—in memory as machine words. The choice of representation profoundly affects performance, memory usage, and implementation complexity. This chapter explains AGENTS' **tagged pointer scheme**, where every value is encoded in a single machine word with type information in the low bits.

## The Tagged Pointer Scheme

### Overview

AGENTS uses a **self-describing** representation: every value contains both data and type information. This is achieved by storing type **tags** in the low bits of pointers and values.

```
┌──────────────────────────────────────────────────────┬─────────┬────┐
│ Payload (56-60 bits)                                 │ Sec Tag │ Pri│
└──────────────────────────────────────────────────────┴─────────┴────┘
 63                                                      3         1  0
```

**Key insight**: On aligned architectures, pointers have low bits zero. By enforcing alignment, we can "borrow" those bits for tags without losing information.

### Primary Tags (2 bits)

The lowest 2 bits (**primary tag**) provide coarse type classification:

| Primary Tag | Value | Meaning |
|-------------|-------|---------|
| **00** | 0 | **Reference** - pointer to term |
| **01** | 1 | **Unbound variable** (unconstrained) |
| **10** | 2 | **Compound** - list or structure (see secondary tag) |
| **11** | 3 | **Immediate or generic** (see secondary tag) |

From `term.h:59-80`:

```c
#define PTagMask  ((uword)0x00000003)   // Mask for primary tag (2 bits)

typedef enum {
    REF = 0,      // 00 - Reference
    UVA = 1,      // 01 - Unbound unconstrained variable
    LST = 2,      // 10 - List cell (with secondary tag)
    GVA = 3       // 11 - Constrained variable or immediate
} tval;
```

### Secondary Tags (4 bits)

When the primary tag is `10` or `11`, the lowest 4 bits (**secondary tag**) refine the type:

| Secondary Tag | Value | Type |
|---------------|-------|------|
| **0010** | 0x2 | List cell [H\|T] |
| **1010** | 0xa | Structure f(...) |
| **1110** | 0xe | Generic object |
| **00110** | 0x6 | Small integer |
| **10110** | 0x16 | Atom |

From `term.h:87-95`:

```c
#define STagMask  ((uword)0x0000000f)   // Mask for secondary tag (4 bits)

#define TAG_LST   ((uword)0x00000002)   // 0010
#define TAG_STR   ((uword)0x0000000a)   // 1010
#define TAG_GEN   ((uword)0x0000000e)   // 1110
#define TAG_ATM   ((uword)0x00000016)   // 10110
#define TAG_INT   ((uword)0x00000006)   // 00110
```

### Complete Type Encoding

Putting it together:

```
Reference:                    pppppppppppppppppppppppppppppp00
Unbound unconstrained var:    pppppppppppppppppppppppppppppp01
Unbound constrained var:      pppppppppppppppppppppppppppp0011
List cell:                    pppppppppppppppppppppppppppp0010
Structure:                    pppppppppppppppppppppppppppp1010
Generic object:               pppppppppppppppppppppppppppp1110
Small integer:                iiiiiiiiiiiiiiiiiiiiiiiiii00110
Atom:                         pppppppppppppppppppppppppppp10110

where:
  p = pointer bits (56-60 bits on 64-bit)
  i = integer value (58 bits on 64-bit)
```

## Word Size: 32-bit vs. 64-bit

### TADBITS Macro

The system adapts to word size via the `TADBITS` macro:

From `sysdeps.h:1-10`:

```c
#if defined(__x86_64__) || defined(__aarch64__) || defined(__alpha__)
#  define TADBITS 64
#else
#  define TADBITS 32
#endif
```

From `term.h:66-70`:

```c
#ifndef TADBITS
#define TADBITS 32
#endif

#define WORDALIGNMENT (TADBITS/8)   // 4 bytes on 32-bit, 8 on 64-bit
```

### Implications

**On 32-bit platforms:**
- Machine word: 32 bits
- Pointer payload: 28-30 bits
- Small integers: 26 bits (-33,554,432 to +33,554,431)
- Address space: 4 GB
- Alignment: 4 bytes

**On 64-bit platforms:**
- Machine word: 64 bits
- Pointer payload: 60 bits
- Small integers: 58 bits (-144,115,188,075,855,872 to +144,115,188,075,855,871)
- Address space: 16 exabytes (effective: ~256 TB userspace)
- Alignment: 8 bytes

The transition to 64-bit dramatically expands the range of small integers, virtually eliminating the need for bignums in typical programs.

## Type Definitions

### Term Type

All values are represented as `Term`:

From `term.h:59-64`:

```c
typedef unsigned long uword;   // Unsigned word
typedef long sword;            // Signed word
typedef uword tagged_address;  // Alias for clarity

typedef uword Term;            // The universal type
```

Every value in AGENTS—atom, variable, structure, list—is a `Term`. This uniform representation simplifies the emulator.

### Opcode Type

Bytecode instructions are represented similarly:

```c
typedef uword opcode;
typedef opcode code;
```

Each instruction is a word containing an opcode and optional immediate operands.

## References (Primary Tag 00)

A **reference** is a pointer to another term:

```
┌──────────────────────────────────────────────────────────┬────┐
│ Address (60 bits)                                        │ 00 │
└──────────────────────────────────────────────────────────┴────┘
```

**Usage:**
- Bound variables point to their values
- Structure arguments point to terms
- List tails point to the rest of the list

**Dereferencing:**

```c
Term Deref(Term t) {
    while (PrimaryTag(t) == REF && t != *AddressOf(t)) {
        t = *AddressOf(t);
    }
    return t;
}
```

Follow references until reaching a non-reference or a self-reference (unbound variable).

### PTR_ORG Adjustment

On some platforms, pointers must be adjusted before tagging:

From `sysdeps.h:17-20`:

```c
#ifdef hppa
#  define PTR_ORG  ((uword)0x40000000)
#endif
```

**Purpose**: If addresses have high bits set that would conflict with tags, subtract `PTR_ORG` before tagging, add it back when dereferencing.

**On modern x86-64/ARM64**: `PTR_ORG = 0` (no adjustment needed).

## Unbound Variables

### Unconstrained Variables (Primary Tag 01)

An unbound variable with no constraints:

```
┌──────────────────────────────────────────────────────────┬────┐
│ Self-address (60 bits)                                   │ 01 │
└──────────────────────────────────────────────────────────┴────┘
```

**Key property**: An unconstrained variable points **to itself**. When you dereference and find `t == *AddressOf(t)` with tag `01`, it's unbound.

**Creation:**

```c
Term MakeVar() {
    Term *cell = AllocateHeap(1);
    *cell = TagUva(cell);   // Point to self with tag 01
    return *cell;
}
```

### Constrained Variables (Secondary Tag 0011)

An unbound variable with constraints (for finite domains, ports, etc.):

```
┌──────────────────────────────────────────────────────────┬────┐
│ Pointer to svainfo structure (60 bits)                  │0011│
└──────────────────────────────────────────────────────────┴────┘
```

The pointer refers to an `svainfo` structure containing:
- Method (constraint solver)
- Environment (variable data)
- Suspension list (agents waiting on this variable)

From `exstate.h:39-47`:

```c
typedef struct svainfo {
    struct svainfo  *forward;    // For GC forwarding
    method          *method;     // Constraint solver
    Term            *env;        // Variable environment
    struct suspend  *susp;       // Suspension list
} svainfo;
```

## Atoms (Secondary Tag 10110)

Atoms are symbolic constants:

```
┌──────────────────────────────────────────────────────────┬────┐
│ Pointer to atom structure (58 bits)                   │10110│
└──────────────────────────────────────────────────────────┴────┘
```

**Atom structure** (from `names.h`):

```c
typedef struct atomstruct {
    char        *name;         // String name
    uword       hash;          // Hash code
    uword       number;        // Unique ID
    struct atomstruct *next;   // Hash table chain
} atom;
```

**Atom table**: All atoms are stored in a global hash table. Identical atom names share the same structure, so atom comparison is pointer comparison (very fast).

**Special atoms:**
- Empty list: `[]` (atom with name "[]")
- True: `true`
- Fail: `fail`

## Small Integers (Secondary Tag 00110)

Small integers are encoded directly:

```
┌──────────────────────────────────────────────────────────┬────┐
│ Integer value (58 bits, signed)                       │00110│
└──────────────────────────────────────────────────────────┴────┘
```

**On 64-bit**, integers from -2^57 to 2^57-1 fit (about ±144 quadrillion).

**On 32-bit**, integers from -2^25 to 2^25-1 fit (about ±33 million).

**Encoding:**

```c
#define SMALLNUMBITS (TADBITS - 6)   // 58 on 64-bit, 26 on 32-bit

Term TagInt(sword i) {
    return (i << 6) | TAG_INT;       // Shift left 6, add tag
}

sword UntagInt(Term t) {
    return (sword)t >> 6;            // Arithmetic shift right 6
}
```

**Overflow**: If an integer exceeds this range, it becomes a **bignum** (generic object) via GMP. On 64-bit, this is exceedingly rare.

## Lists (Secondary Tag 0010)

A list cell `[H|T]` is represented as:

```
┌──────────────────────────────────────────────────────────┬────┐
│ Pointer to two-word structure (60 bits)                │0010│
└──────────────────────────────────────────────────────────┴────┘
```

The pointer refers to a two-word block:

```
┌────────────┐
│ Head term  │  (Term)
├────────────┤
│ Tail term  │  (Term)
└────────────┘
```

**Macros** (from `term.h`):

```c
#define Car(t)  (AddressOf(t)[0])    // Head
#define Cdr(t)  (AddressOf(t)[1])    // Tail

Term MakeList(Term head, Term tail) {
    Term *cell = AllocateHeap(2);
    cell[0] = head;
    cell[1] = tail;
    return TagLst(cell);
}
```

**Example**: List `[1, 2, 3]` is `[1 | [2 | [3 | []]]]`:

```
┌─────┬─────┐       ┌─────┬─────┐       ┌─────┬─────┐
│  1  │  ●──┼──────>│  2  │  ●──┼──────>│  3  │ []  │
└─────┴─────┘       └─────┴─────┘       └─────┴─────┘
```

Each cell is tagged with `0010`.

## Structures (Secondary Tag 1010)

A structure `f(a, b, c)` is represented as:

```
┌──────────────────────────────────────────────────────────┬────┐
│ Pointer to structure block (60 bits)                    │1010│
└──────────────────────────────────────────────────────────┴────┘
```

The pointer refers to a block:

```
┌─────────────────┐
│ Functor pointer │ (tagged pointer to functor structure)
├─────────────────┤
│ Arg 1           │ (Term)
├─────────────────┤
│ Arg 2           │ (Term)
├─────────────────┤
│ Arg 3           │ (Term)
└─────────────────┘
```

**Functor structure** (from `functor.h`):

```c
typedef struct functorstruct {
    atom   *name;          // Functor name (atom)
    uword  arity;          // Number of arguments
    uword  hash;           // Hash code
    struct functorstruct *next;  // Hash table chain
} functor;
```

**Macros**:

```c
#define Functor(t)   (*(functor**)AddressOf(t))
#define Arg(t, i)    (AddressOf(t)[i])   // 1-indexed
```

**Example**: `point(10, 20)` in memory:

```
┌──────────────┐
│ ptr to       │
│ point/2      │
│ functor      │
├──────────────┤
│ TagInt(10)   │
├──────────────┤
│ TagInt(20)   │
└──────────────┘
```

## Generic Objects (Secondary Tag 1110)

Generic objects handle types that don't fit the other categories:

```
┌──────────────────────────────────────────────────────────┬────┐
│ Pointer to generic structure (60 bits)                  │1110│
└──────────────────────────────────────────────────────────┴────┘
```

**Uses:**
- **Bignums** (arbitrary-precision integers via GMP)
- **Floats** (double-precision floating point)
- **Rationals** (numerator/denominator pairs)
- **Ports** (write-protected streams)
- **Foreign objects** (user-defined types)

**Generic structure** (from `term.h`):

```c
typedef struct genobstruct {
    struct genobstruct *forward;  // For GC forwarding
    int   type;                    // Type code (bignum, float, etc.)
    union {
        double floating;
        struct {
            Term numerator;
            Term denominator;
        } rational;
        struct {
            char *data;
            int length;
        } blob;
        // ... more variants
    } value;
} genob;
```

The `type` field dispatches operations to type-specific code.

## Memory Layout

### Heap Allocation

Terms are allocated on the heap. The heap is organized into **blocks** managed by the allocator:

```
Heap Block:
┌─────────────────────────────────────────────────────┐
│ Block header (flipflop state, pointers, etc.)      │
├─────────────────────────────────────────────────────┤
│                                                     │
│              Allocated terms                        │
│                                                     │
│  [List cell] [Structure] [Atom] [Variable] ...     │
│                                                     │
├─────────────────────────────────────────────────────┤
│               Free space                            │
└─────────────────────────────────────────────────────┘
```

**Bump-pointer allocation**: To allocate N words, increment heap pointer by N*WORDALIGNMENT. Fast and simple.

### Constant Space

Atoms and functors are allocated in **constant space**, not the heap. They are **never garbage collected** (they're immortal).

This separation simplifies GC: terms on the heap may reference atoms, but atoms never reference heap objects.

### Alignment

All term pointers must be aligned to `WORDALIGNMENT` (8 bytes on 64-bit):

```c
#define WordAlign(x) \
    (((x) + (WORDALIGNMENT-1)) & ~((uword)(WORDALIGNMENT-1)))
```

**Why**: To keep low bits zero for tags.

**Enforcement**: The allocator rounds all sizes up to multiples of WORDALIGNMENT.

## Tag Manipulation Macros

From `term.h`, AGENTS provides comprehensive macros:

**Tagging:**
```c
#define TagRef(p)   ((Term)((uword)(p) - PTR_ORG))
#define TagUva(p)   ((Term)(((uword)(p) - PTR_ORG) | UVA))
#define TagLst(p)   ((Term)(((uword)(p) - PTR_ORG) | TAG_LST))
#define TagStr(p)   ((Term)(((uword)(p) - PTR_ORG) | TAG_STR))
#define TagAtm(p)   ((Term)(((uword)(p) - PTR_ORG) | TAG_ATM))
#define TagInt(i)   ((Term)(((sword)(i) << 6) | TAG_INT))
#define TagGva(p)   ((Term)(((uword)(p) - PTR_ORG) | GVA | (GVA<<2)))
#define TagGen(p)   ((Term)(((uword)(p) - PTR_ORG) | TAG_GEN))
```

**Untagging:**
```c
#define AddressOf(t)   ((Term*)((uword)(t) + PTR_ORG))
#define UntagInt(t)    ((sword)(t) >> 6)
```

**Type testing:**
```c
#define IsRef(t)       ((PrimaryTag(t) == REF) && ((t) != *(Term*)AddressOf(t)))
#define IsUva(t)       ((PrimaryTag(t) == UVA) && ((t) == *(Term*)AddressOf(t)))
#define IsGva(t)       ((SecondaryTag(t) == (GVA | (GVA<<2))))
#define IsLst(t)       (SecondaryTag(t) == TAG_LST)
#define IsStr(t)       (SecondaryTag(t) == TAG_STR)
#define IsAtm(t)       (SecondaryTag(t) == TAG_ATM)
#define IsInt(t)       (SecondaryTag(t) == TAG_INT)
#define IsGen(t)       (SecondaryTag(t) == TAG_GEN)
```

These macros are used pervasively in the emulator.

## Dereferencing

**Dereferencing** follows reference chains to find the ultimate value:

From `term.h:500-520`:

```c
#define Deref(t) \
do { \
    while (PrimaryTag(t) == REF) { \
        Term *addr = AddressOf(t); \
        if (*addr == t) break;   /* Unbound variable */ \
        t = *addr; \
    } \
} while (0)
```

**Why loop**: Variables can form chains `X -> Y -> Z -> unbound`. We must follow all links.

**Termination**: Either we reach a non-reference, or we reach a self-reference (unbound variable).

## The GC Mark Bit

The high bit (bit 63 on 64-bit, bit 31 on 32-bit) is reserved for **garbage collection marking**:

```
┌─┬──────────────────────────────────────────────────────┬─────────┬────┐
│M│ Payload                                             │ Sec Tag │ Pri│
└─┴──────────────────────────────────────────────────────┴─────────┴────┘
 63                                                       3         1  0

M = Mark bit (used during GC)
```

During GC:
- **Mark phase**: Set mark bit on live objects
- **Sweep phase**: Reclaim unmarked objects
- **Forwarding**: Use marked bit to indicate forwarded pointers

This bit is transparent to application code—only the GC manipulates it.

## Examples

### Example 1: Representing `[1, 2, 3]`

In memory:

```
Cell 1 (address 0x1000):
  [0]: TagInt(1)         = 0x000000000000004006
  [1]: TagLst(0x1010)    = 0x0000000000001012

Cell 2 (address 0x1010):
  [0]: TagInt(2)         = 0x0000000000000008006
  [1]: TagLst(0x1020)    = 0x0000000000001022

Cell 3 (address 0x1020):
  [0]: TagInt(3)         = 0x00000000000000C006
  [1]: TagAtm(nil)       = 0x0000...16  (pointer to [] atom)

List = TagLst(0x1000)   = 0x0000000000001002
```

### Example 2: Representing `point(X, Y)` where X and Y are unbound

```
Functor (in constant space):
  name: pointer to "point" atom
  arity: 2

Structure (address 0x2000):
  [0]: pointer to point/2 functor
  [1]: TagUva(0x2008)    = 0x0000000000002009  (X, self-reference)
  [2]: TagUva(0x2010)    = 0x0000000000002011  (Y, self-reference)

Structure term = TagStr(0x2000) = 0x000000000000200A
```

When X is later bound to 5:
- Cell at 0x2008 is updated: `0x000000000000014006` (TagInt(5))

## Design Tradeoffs

### Advantages of Tagged Pointers

1. **Compact**: One word per value, no separate type field needed
2. **Fast type checking**: Just mask and compare bits
3. **Direct encoding**: Small integers and atoms are values, not pointers
4. **Cache-friendly**: Less indirection, better locality
5. **Self-describing**: Every value carries its type

### Disadvantages

1. **Limited integer range**: On 32-bit, only ±33 million
2. **Alignment requirement**: All structures must be aligned
3. **Pointer adjustment**: On some platforms, need PTR_ORG
4. **Bit manipulation**: More complex code than naive pointers
5. **Platform dependence**: Word size affects representation

### Why AGENTS Uses This Scheme

Historical context: In the early 1990s, memory was expensive and CPUs were slow. Tagged pointers provided:
- **Memory efficiency**: No extra words for types
- **Speed**: Type tests are bitwise operations, very fast

Modern relevance: Even today, this scheme is competitive. Many VMs (V8, LuaJIT, CPython) use similar tagged pointer techniques.

The transition to 64-bit makes this scheme even more attractive: the expanded integer range eliminates most bignum needs.

## Summary

AGENTS' data representation is a sophisticated tagged pointer scheme:

- **All data fits in one word** with type tags in low bits
- **Primary tag** (2 bits) for coarse classification
- **Secondary tag** (4 bits) for refinement
- **58-bit small integers** on 64-bit (huge range)
- **Self-referencing unbound variables**
- **Aligned heap allocation** to keep low bits zero
- **Constant space for atoms/functors** (never GC'd)
- **Mark bit for GC** in high bit

This design enables fast type checking, compact memory use, and efficient emulator implementation.

Understanding data representation is crucial for the next topics: memory management, unification, and the execution engine.

---

**Next Chapter**: [Chapter 7: Memory Management](chapter-07-memory-management.md)
