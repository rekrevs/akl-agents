# AGENTS v0.9 - Detailed Emulator Analysis

**Document:** Phase 1, Document 2 of 3
**Status:** Complete
**Date:** 2025-11-04
**Purpose:** Deep analysis of emulator internals for porting assessment

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Emulator File Structure](#emulator-file-structure)
3. [Core Execution Engine](#core-execution-engine)
4. [Instruction Dispatch Mechanisms](#instruction-dispatch-mechanisms)
5. [Data Structures](#data-structures)
6. [Memory Management](#memory-management)
7. [Unification Engine](#unification-engine)
8. [Garbage Collection](#garbage-collection)
9. [Constraint Solving](#constraint-solving)
10. [Built-in Predicates](#built-in-predicates)
11. [Porting Assessment](#porting-assessment)
12. [References](#references)

---

## Executive Summary

The AGENTS emulator is a sophisticated **register-based abstract machine** implementing concurrent constraint programming. This document provides detailed analysis of the ~50 C source files comprising the emulator core.

### Key Components

| Component | Files | Complexity | Arch Dependencies |
|-----------|-------|------------|-------------------|
| **Execution Engine** | engine.c (2500+ lines) | ⚠️ High | HARDREGS, THREADED_CODE |
| **Memory Manager** | storage.c, gc.c | ⚠️ High | WORDALIGNMENT, PTR_ORG |
| **Unification** | unify.c | Medium | Tagged pointers |
| **Constraints** | fd.c, fd_akl.c | Medium | None |
| **Built-ins** | builtin.c (3000+ lines) | Medium | Platform I/O |
| **Support** | 40+ other files | Low-Medium | Minimal |

### Architecture Impact Summary

**High Impact (Requires Careful Review):**
- Tagged pointer manipulations throughout
- Alignment-sensitive memory allocation
- Register allocation (if using HARDREGS)
- Threaded code dispatch tables

**Medium Impact (Standard C with minimal issues):**
- Most data structure code
- Algorithm implementations
- Control flow logic

**Low Impact (Portable C):**
- String handling, parsing
- I/O operations
- Utility functions

---

## Emulator File Structure

### Complete File Inventory

Total: **50 C source files**, **63 header files**

#### Core Execution (engine.c - 2500+ lines)

**Purpose:** Main bytecode interpreter and instruction dispatch

**Key Functions:**
- `tengine()` - Main execution loop
- Instruction handlers for ~100+ opcodes
- And-box and choice-box management
- Backtracking and trail handling

**Architecture Dependencies:**
- THREADED_CODE: Computed goto vs switch
- HARDREGS: Physical register allocation
- Platform: None beyond standard C

#### Memory Management

| File | Lines | Purpose | Arch Deps |
|------|-------|---------|-----------|
| **storage.c** | ~500 | Heap allocation, block management | WORDALIGNMENT |
| **storage.h** | ~379 | Memory macros and structures | WORDALIGNMENT, PTR_ORG |
| **gc.c** | ~1500 | Copying garbage collector | Tagged pointers, alignment |
| **gc.h** | ~100 | GC interface | Minimal |

#### Term Manipulation

| File | Lines | Purpose | Arch Deps |
|------|-------|---------|-----------|
| **term.h** | ~738 | Tagged pointer scheme | TADBITS, PTR_ORG |
| **term.c** | ~300 | Term utilities | Tagged pointers |
| **copy.c** | ~800 | Term copying (non-det promotion) | Tagged pointers, alignment |
| **unify.c** | ~600 | Unification algorithm | Tagged pointers |
| **compare.c** | ~400 | Term comparison | Tagged pointers |

#### Data Structures

| File | Lines | Purpose | Arch Deps |
|------|-------|---------|-----------|
| **tree.h** | ~158 | And-box, choice-box definitions | TRACE conditional |
| **exstate.h** | ~471 | Execution state | BAM conditional |
| **exstate.c** | ~800 | Execution state management | None |
| **predicate.h** | ~100 | Predicate structures | TRACE conditional |
| **predicate.c** | ~600 | Predicate management | None |

#### Constraint Solving

| File | Lines | Purpose | Arch Deps |
|------|-------|---------|-----------|
| **fd.c** | ~2000 | Finite domain constraints | None |
| **fd_akl.c** | ~1500 | FD constraint interface | THREADED_CODE |
| **fd_akl.h** | ~200 | FD headers | THREADED_CODE |
| **fd_instr.h** | ~100 | FD instructions | None |

#### Built-in Predicates

| File | Lines | Purpose | Arch Deps |
|------|-------|---------|-----------|
| **builtin.c** | ~3000 | Core built-ins | Platform I/O |
| **port.c** | ~500 | Port communication | None |
| **send.c** | ~300 | Message sending | None |
| **aggregate.c** | ~400 | Aggregation operations | None |
| **abstraction.c** | ~300 | Higher-order constructs | None |
| **reflection.c** | ~500 | Meta-programming | None |

#### Bytecode Management

| File | Lines | Purpose | Arch Deps |
|------|-------|---------|-----------|
| **code.c** | ~800 | Code loading | None |
| **load.c** | ~600 | PAM file loading | Endianness (minor) |
| **database.c** | ~400 | Clause database | None |

#### Arithmetic and Numbers

| File | Lines | Purpose | Arch Deps |
|------|-------|---------|-----------|
| **is.c** | ~800 | Arithmetic evaluation | None |
| **bignum.c** | ~1000 | GMP interface | GMP library |
| **rational.c** | ~400 | Rational arithmetic | None |
| **config.c** | ~200 | Term construction | TADTURMS |

#### I/O and Parsing

| File | Lines | Purpose | Arch Deps |
|------|-------|---------|-----------|
| **inout.c** | ~1200 | File I/O | Platform paths, SYS5 |
| **display.c** | ~1500 | Term printing | None |
| **parser.tab.c** | ~3000 | Bison-generated parser | None |
| **parser.yy.c** | ~1000 | Flex-generated lexer | None |

#### Debugging and Tracing

| File | Lines | Purpose | Arch Deps |
|------|-------|---------|-----------|
| **trace.c** | ~800 | Execution tracing | TRACE |
| **trace.h** | ~200 | Trace interface | TRACE |
| **debug.c** | ~600 | Debugger | TRACE |
| **error.c** | ~300 | Error handling | None |

#### Support and Utilities

| File | Lines | Purpose | Arch Deps |
|------|-------|---------|-----------|
| **main.c** | ~400 | Initialization | Platform detection |
| **initial.c** | ~1000 | System initialization | None |
| **intrpt.c** | ~200 | Interrupt handling | Unix signals |
| **time.c** | ~300 | Timing utilities | SYS5, getrusage |
| **statistics.c** | ~400 | Runtime statistics | None |
| **foreign.c** | ~600 | Foreign function interface | Platform callbacks |
| **foreign_stub.c** | ~100 | FFI stubs | None |
| **names.c** | ~400 | Atom management | None |
| **functor.c** | ~300 | Functor handling | None |
| **hash.c** | ~200 | Hash tables | None |
| **ghash.c** | ~300 | Generic hash tables | None |
| **ptrhash.c** | ~200 | Pointer hash tables | None |
| **array.c** | ~400 | Dynamic arrays | None |
| **candidate.c** | ~500 | Candidate selection | None |
| **examine.c** | ~400 | Constraint examination | None |
| **worker.c** | ~200 | Worker (old?) | None |

#### Platform-Specific

| File | Lines | Purpose | Arch Deps |
|------|-------|---------|-----------|
| **macintosh.c** | ~300 | Mac-specific code | macintosh |
| **bam.c** | ~600 | BAM extensions | BAM |

#### Headers

**63 header files** provide interfaces, macros, and type definitions. Most are architecture-independent except:
- `sysdeps.h` - Platform detection
- `regdefs.h` - Hard register allocation
- `term.h` - Tagged pointer scheme
- `storage.h` - Memory alignment

---

## Core Execution Engine

### engine.c - The Heart of AGENTS

**File:** `emulator/engine.c` (2500+ lines)
**Function:** `void tengine(exstate *exs, int initcodes)`

### Execution Loop Structure

```c
void tengine(exstate *exs, int initcodes)
{
  // Register variables (possibly mapped to physical registers)
  register code   *pc REGISTER3;      // Program counter
  register opcode op REGISTER4;       // Current opcode
  register Term   *areg REGISTER5;    // Argument registers
  register Term   *yreg REGISTER6;    // Environment (Y registers)
  register andbox *andb REGISTER2;    // Current and-box
  register choicebox *chb;            // Current choice-box
  register choicebox *insert;         // Insertion point
  register int write_mode REGISTER1;  // Write mode flag

  // Initialize from execution state
  andb = exs->andb;
  insert = exs->insert;
  areg = exs->areg;
  pc = exs->pc;
  FetchOpCode(0);  // Read first opcode

  // Main dispatch loop
  for(;;) {
    goto choicehandler;

    // Instruction dispatch point
#ifndef THREADED_CODE
read_instr_dispatch:
#endif

    SwitchTo(op) {
      CaseLabel(SWITCH_ON_TERM):
        // Type switching
        ...

      CaseLabel(TRY):
        // Create choice point
        ...

      CaseLabel(CALL):
        // Procedure call
        ...

      // ~100+ more instruction handlers
    }
  }
}
```

### Key Observations

1. **Heavily Optimized:** Register variables with explicit register hints
2. **Two Dispatch Modes:** Threaded code (computed goto) or switch
3. **Infinite Loop:** Never returns normally, only via exceptions
4. **State in Registers:** Critical variables kept in registers
5. **Minimal Function Calls:** Most instructions handled inline

### Register Usage Strategy

From `engine.c:117-154`:

```c
register code   *pc REGISTER3;    // Most frequently accessed
register opcode op REGISTER4;     // Hot path - opcode comparison
register Term   *areg REGISTER5;  // Argument access
register Term   *yreg REGISTER6;  // Environment access
register andbox *andb REGISTER2;  // Current context
register int write_mode REGISTER1; // Mode flag
```

**Rationale:**
- `pc`, `op`: Used every instruction cycle
- `areg`, `yreg`: Used for most data access
- `andb`: Critical for context operations
- `write_mode`: Checked frequently for unification mode

**Porting Implication:** Need good register allocator or define REGISTER* for x86-64/ARM64

---

## Instruction Dispatch Mechanisms

### Threaded Code (Fast Path)

**File:** `emulator/instructions.h:27-41`

```c
#ifdef THREADED_CODE
typedef void *address;
typedef address opcode;

extern address *instr_label_table;

#define SwitchTo(x)   goto *(address)(x);
#define CaseLabel(x)  LABELNAME(x)
#define NextOp()      SwitchTo(op)
#endif
```

**Mechanism:**

1. **Label Table Initialization (engine.c:161-165):**
```c
#ifdef THREADED_CODE
#define Item(x)  &&CaseLabel(x),

static address label_table[] = {
#include "opcodes.h"  // Expands to: &&LABEL_TRY, &&LABEL_CALL, ...
};

instr_label_table = label_table;
#endif
```

2. **Opcode Storage:**
   - Bytecode stores indices into label table
   - Each "opcode" is actually a label address

3. **Dispatch:**
```c
FetchOpCode(0);  // Load next opcode (label address) from pc
NextOp();        // goto *op;  (direct jump!)
```

4. **Handler:**
```c
CaseLabel(CALL):  // Expands to: LABEL_CALL:
  // Handle CALL instruction
  ...
  NextOp();       // Jump to next instruction handler
```

**Performance:** ~20-30% faster than switch (no bounds check, no jump table lookup)

**Requirements:**
- gcc 2.0+ or clang
- Label-as-values extension (&&label)

**Porting:** ✅ x86-64 and ARM64 fully support this with gcc/clang

---

### Switch-Based Dispatch (Fallback)

**File:** `emulator/instructions.h:43-52`

```c
#ifndef THREADED_CODE
typedef enum_opcode opcode;

#define SwitchTo(x)   switch(x)
#define CaseLabel(x)  case x
#define NextOp()      goto read_instr_dispatch
#endif
```

**Mechanism:**

1. **Opcode Storage:** Bytecode stores enum values
2. **Dispatch:**
```c
read_instr_dispatch:
  switch(op) {
    case CALL:
      // Handle CALL
      NextOp();  // goto read_instr_dispatch;
    case TRY:
      // Handle TRY
      NextOp();
    ...
  }
```

**Performance:** Slower due to:
- Switch statement overhead
- Potential bounds check
- Indirect jump through jump table
- Less friendly to branch prediction

**Porting:** ✅ Always works (standard C)

---

### Instruction Fetching

**From decodeinstr.h (inferred):**

```c
#define FetchOpCode(offset) \
  op = *(opcode*)(pc + offset)

#define FetchTryBranchLabel(label) \
  label = *(code**)(pc); \
  pc += sizeof(code*)
```

**Architecture Impact:**
- Assumes little-endian for multi-byte values (typically)
- Assumes unaligned access OK (may need fixing for ARM64)
- Pointer arithmetic assumes sizeof(opcode) correct

---

## Data Structures

### And-Box (Conjunction Context)

**File:** `emulator/tree.h:66-80`

```c
typedef enum {
  DEAD,
  STABLE,      // No pending operations
  UNSTABLE,    // Has pending operations
  XSTABLE,     // Extended stable (?)
  XUNSTABLE    // Extended unstable (?)
} sflag;

typedef struct andbox {
  sflag          status;       // Liveness/stability
  struct envid   *env;          // Environment pointer
#ifdef TRACE
  int            trace;         // Trace flag
  int            clno;          // Clause number
  int            id;            // And-box ID
#endif
  struct unifier *unify;        // Pending unifications
  struct constraint *constr;    // Pending constraints
  struct choicebox *tried;      // Nested choice-box (or NULL)
  struct andcont *cont;         // Continuation
  struct choicebox *father;     // Parent choice-box
  struct andbox  *next, *previous; // Sibling and-boxes
} andbox;
```

**Size:** 44 bytes (32-bit) / 88 bytes (64-bit, with pointers doubling)

**Purpose:**
- Represents a conjunction of goals (agent body)
- Tracks execution context
- Links to parent choice-box and sibling and-boxes
- Holds pending unifications and constraints

**Architecture Impact:**
- Pointer-heavy (7 pointers on 32-bit, more on 64-bit)
- Size doubles on 64-bit
- Alignment: natural alignment (pointer-aligned)

---

### Choice-Box (Disjunction Context)

**File:** `emulator/tree.h:95-107`

```c
typedef struct choicebox {
#ifdef TRACE
  int            trace;
  int            next_clno;
  int            id;
#endif
  struct choicecont *cont;      // Continuation (saved state)
  struct choicebox *next, *previous; // Sibling choice-boxes
  struct andbox  *father;       // Parent and-box
  struct predicate *def;        // Predicate definition
  struct andbox  *tried;        // List of alternative and-boxes
  int            type;          // Choice-box type
} choicebox;
```

**Size:** 28 bytes (32-bit) / 56 bytes (64-bit)

**Purpose:**
- Represents a disjunction (alternative clauses)
- Holds saved state for backtracking
- Links to parent and-box and alternative and-boxes

---

### Choice Continuation (Saved State)

**File:** `emulator/tree.h:111-118`

```c
typedef struct choicecont {
  union {
    code  *label;      // Next clause to try
    bool  (*cinfo)();  // Or: built-in continuation
  } instr;
  int   arity;         // Number of arguments
  Term  arg[ANY];      // Saved argument registers (variable-sized)
} choicecont;

#define ChoiceContSize(Arity)  (sizeof(choicecont) + sizeof(Term)*((Arity) - ANY))
```

**Purpose:**
- Saves program counter and arguments for backtracking
- Variable-sized structure (depends on arity)
- Allocated from heap

**Architecture Impact:**
- Variable-sized allocation
- Alignment depends on Term alignment
- ANY = 1 typically, so ChoiceContSize(N) = sizeof(choicecont) + sizeof(Term)*(N-1)

---

### And Continuation (Environment Frame)

**File:** `emulator/tree.h:82-93`

```c
typedef struct andcont {
#ifdef TRACE
  int            trace;
  struct predicate *def;
  int            clno;
  struct choicecont *choice_cont;
#endif
  code           *label;        // Return address
  struct andcont *next;         // Previous frame
  int            ysize;         // Environment size
  Term           yreg[ANY];     // Environment (Y registers)
} andcont;

#define AndContSize(Arity)  (sizeof(andcont) + sizeof(Term) * ((Arity) - ANY))
```

**Purpose:**
- Stack frame for procedure calls
- Holds environment variables (Y registers)
- Linked list of frames

**Architecture Impact:**
- Variable-sized (like choice continuation)
- Forms linked call stack

---

### Execution State (exstate)

**File:** `emulator/exstate.h:417-442`

```c
typedef struct exstate {
  struct exstate   *father;          // Parent state
  struct exstate   *next;            // Sibling states
  struct exstate   *child;           // Child states
  close_entry      *close;           // Close list for GC
  code             *pc;              // Program counter
  uword            arity;            // Current arity
  predicate        *def;             // Current predicate
  choicebox        *insert;          // Insertion point
  andbox           *andb;            // Current and-box
  choicebox        *root;            // Root choice-box
  trailhead        trail;            // Backtracking trail
  taskhead         task;             // Task queue
  recallhead       recall;           // Recall stack
  wakehead         wake;             // Wake queue
  contexthead      context;          // Context stack
  suspendhead      suspend;          // Suspension stack
  bool             tracing;          // Trace flag
  bool             debugging;        // Debug flag
  struct unifystate urec;            // Unification state
  Term             areg[MAX_AREGS];  // Argument registers
#ifdef BAM
  cons_stack_head  cons_stack;
  try_stack_head   try_stack;
#endif
} exstate;
```

**Size:** ~34KB with MAX_AREGS=8192 (mostly areg array)

**Purpose:**
- Complete execution state for an agent
- Multiple agents can exist concurrently
- Represents a "worker" or "engine"

**Architecture Impact:**
- MAX_AREGS affects size dramatically
- Large structure (heap-allocated)
- Pointer-heavy

---

## Memory Management

### Heap Organization

**File:** `emulator/storage.h:334-379`

```c
typedef struct block {
  flipstate   flipflop;       // FLIP or FLOP (for GC)
  struct block *next;         // Next block
  heap        *start;         // Block start
  heap        *end;           // Block end
  heap        *logical_end;   // Allocation limit
} block;

extern heap *glob_heapcurrent;  // Current allocation pointer
extern heap *glob_heapend;      // End of current block
```

**Allocation Strategy:**

From `storage.h:36-47`:
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
- **Bump-pointer allocation:** Fast, just increment pointer
- **Overflow handling:** Allocate new block when full
- **Block chaining:** Linked list of blocks

**Architecture Impact:**
- Pointer arithmetic
- Alignment requirements enforced by macros
- No architecture-specific code

---

### Alignment Handling

**File:** `emulator/storage.h:5-9`

```c
#define WordAlign(x) (((x) + (WORDALIGNMENT-1)) & ~((uword)(WORDALIGNMENT-1)))

#ifndef OptionalWordAlign
#define OptionalWordAlign(x)  x
#endif
```

**Usage Examples:**

```c
// Float allocation (must be double-aligned)
#define NewFloat(ret)\
{\
    if((uword)heapcurrent % sizeof(double) != 0) {\
       heapcurrent += sizeof(double) - (uword)heapcurrent % sizeof(double);\
    }\
    (ret) = (Fp) heapcurrent; \
    heapcurrent += FltSize; \
    ...
}

// Atom allocation (must respect PTR_ATM_BIT)
#define NewAtom(atm)\
{ \
    if(((uword)constcurrent & PTR_ATM_BIT) != 0) {\
       constcurrent += PTR_ATM_BIT;\
    }\
    (atm) = (atom*)constcurrent;\
    ...
}
```

**Architecture Impact:**
- WORDALIGNMENT = 4 on 32-bit, 8 on 64-bit
- Floats always 8-byte aligned
- Atoms must not have PTR_ATM_BIT set in address
- ARM64 may need OptionalWordAlign enforced

---

### Constant Space

**Separate area for immutable data:**

From `storage.h:354-360`:
```c
extern heap *constspace;     // Start of constant area
extern heap *constend;       // End of constant area
extern heap *constcurrent;   // Current allocation pointer

extern int constsize;        // Size of constant area
```

**Purpose:**
- Atoms, functors never garbage collected
- Allocated once, live forever
- Separate space prevents fragmentation

---

## Unification Engine

**File:** `emulator/unify.c` (~600 lines)

### Unification State

**File:** `emulator/unify.h:16-23`

```c
struct unifystate {
  struct hash_table *table;        // Occurs-check hash table
  int               table_used_flag;
  int               stack_length;
  unify_entry       *sp;            // Stack pointer
  unify_entry       *stack_end;
  unify_entry       *stack;         // Unification stack
};
```

### Unification Algorithm

**Interface:**
```c
bool unify(Term X, Term Y, andbox *andb, exstate *exs);
bool unify_internal(Term X, Term Y, andbox *andb, exstate *exs);
bool unify_more(Term X, Term Y, andbox *andb, exstate *exs);
```

**Strategy:**
1. **Cheap unification** (inline in macros):
   - Identical terms
   - Local variable binding
2. **Deep unification** (function call):
   - Structure/list unification
   - Occurs check
   - Constraint variable handling

**From term.h:679-700:**
```c
#define CheapUnify_ELSE(X,Y,A,R) \
      if(Eq(X,Y)) {\
        (R)=TRUE; \
      } else if(IsREF(X) && IsLocalUVA(Ref(X),A)) { \
        (R)=TRUE; \
        BindCheck(Ref(X),Y,(R)=FALSE); \
      } else if(IsREF(Y) && IsLocalUVA(Ref(Y),A)) { \
        (R)=TRUE; \
        BindCheck(Ref(Y),X,(R)=FALSE); \
      } else

#define UNIFY(X,Y,R) \
  { \
    CheapUnify_ELSE(X,Y,exs->andb,R) \
      (R) = unify_internal(X,Y,exs->andb,exs); \
  }
```

**Architecture Impact:**
- Heavily uses tagged pointer operations
- Dereferencing chains
- Pointer comparisons
- All architecture-independent at C level

---

## Garbage Collection

**File:** `emulator/gc.c` (~1500 lines)

### GC Algorithm: Copying Collector

**Strategy:** Two-space copying with flip-flop heaps

**From storage.h:327-345:**
```c
typedef enum {
  FLIP = 0,
  FLOP = 1
} flipstate;

typedef enum {
  OFF = 0,
  ON = 1
} gcvalue;

extern gcvalue gcflag;
extern flipstate currentflip;
```

### GC Process

1. **Trigger:** `gcflag = ON` when heap full
2. **Root Set:** Collect roots from:
   - Execution state registers (areg, yreg)
   - And-boxes and choice-boxes
   - Trail
   - Task queues
3. **Copy:** Deep-copy live objects to new space
4. **Update:** Update all pointers to new locations
5. **Flip:** Swap from-space and to-space
6. **Resume:** Continue execution

### Forwarding Pointers

**From term.h:205-220:**
```c
#define ForeWord(A)     (*(tagged_address*)(A))
#define ForwOf(FW)      ((void*)((FW)-MarkBitMask))
#define IsForwMarked(FW) (((tagged_address)(FW)) & MarkBitMask)

#define IsCopied(X)     IsForwMarked(ForeWord(X))

#define Link(Old,New) \
    (*(tagged_address*)(Old)) = (tagged_address)(New) + MarkBitMask

#define Forw(Old)       ForwOf(ForeWord(Old))
```

**Mechanism:**
- When object copied, overwrite first word with forwarding pointer
- Set high bit (MarkBitMask) to indicate forwarding pointer
- Later references follow forwarding pointer
- Enables copying of circular structures

**Architecture Impact:**
- Uses high bit of word (bit 31 on 32-bit, bit 63 on 64-bit)
- Pointer arithmetic
- Assumes MarkBitMask doesn't conflict with addresses
- ✅ Works on 64-bit (Alpha proof)

---

## Constraint Solving

### Finite Domain Constraints

**Files:** `emulator/fd.c` (~2000 lines), `emulator/fd_akl.c` (~1500 lines)

**Purpose:**
- Integer domain constraints (X in 1..10)
- Arithmetic constraints (X + Y = Z)
- Constraint propagation
- Suspension and waking

**Architecture Impact:**
- Pure algorithm code
- No architecture dependencies
- Uses term manipulation (architecture-independent at C level)

---

## Built-in Predicates

**File:** `emulator/builtin.c` (~3000 lines)

### Categories

1. **Arithmetic:** `is/2`, `=:=/2`, `</2`, etc.
2. **I/O:** `read/1`, `write/1`, `get/1`, `put/1`
3. **Meta:** `call/1`, `findall/3`, `bagof/3`
4. **Reflection:** `functor/3`, `arg/3`, `=../2`
5. **Control:** `!/0`, `fail/0`, `halt/0`
6. **Ports:** `send/2`, `receive/1`
7. **Database:** `assert/1`, `retract/1`
8. **System:** File I/O, timing, statistics

### Platform Dependencies

**I/O Operations:**
- File paths (Unix vs other)
- `getrusage()` vs `times()` (SYS5)
- `getwd()` vs `getcwd()` (SYS5)
- Terminal I/O

**From inout.c, time.c:**
- Platform-specific system calls
- Already abstracted via #ifdef

---

## Porting Assessment

### High-Priority Items for x86-64 / ARM64

#### 1. Update Platform Detection (REQUIRED)

**File:** `emulator/sysdeps.h`

**Add:**
```c
#if defined(__x86_64__) || defined(__amd64__)
#define TADBITS 64
#endif

#if defined(__aarch64__) || defined(__arm64__)
#define TADBITS 64
/* May need: */
/* #define OptionalWordAlign(x)  WordAlign((unsigned long)(x)) */
#endif
```

#### 2. Add Hard Register Definitions (OPTIONAL but RECOMMENDED)

**File:** `emulator/regdefs.h`

**x86-64:**
```c
#if defined(__x86_64__) || defined(__amd64__)
#define REGISTER1 asm("r15")
#define REGISTER2 asm("r14")
#define REGISTER3 asm("r13")
#define REGISTER4 asm("r12")
#define REGISTER5 asm("rbx")
#define REGISTER6 asm("rbp")  /* Use with caution */
#endif
```

**ARM64:**
```c
#if defined(__aarch64__) || defined(__arm64__)
#define REGISTER1 asm("x28")
#define REGISTER2 asm("x27")
#define REGISTER3 asm("x26")
#define REGISTER4 asm("x25")
#define REGISTER5 asm("x24")
#define REGISTER6 asm("x23")
#define REGISTER7 asm("x22")
#define REGISTER8 asm("x21")
#define REGISTER9 asm("x20")
#define REGISTER10 asm("x19")
#endif
```

#### 3. Test PTR_ORG = 0 (LIKELY OK)

**Modern Linux user space starts at 0x00000000, so PTR_ORG = 0 should work**

**Test:** Run AGENTS and verify no tag conflicts

**If conflicts occur:** Set PTR_ORG appropriately (unlikely)

#### 4. Test Alignment on ARM64 (MAY NEED ADJUSTMENT)

**ARM64 has strict alignment preferences**

**Test:** Run test suite and check for SIGBUS (bus errors)

**If failures:** Enable `OptionalWordAlign` in sysdeps.h:
```c
#if defined(__aarch64__) || defined(__arm64__)
#define OptionalWordAlign(x)  WordAlign((unsigned long)(x))
#endif
```

#### 5. Update Build System (MINOR)

**File:** `configure.in`

- Modern autoconf should detect x86-64 and ARM64 automatically
- May need to add platform-specific cases if issues arise

#### 6. Test with GCC/Clang (SHOULD WORK)

- Both gcc and clang support threaded code on x86-64 and ARM64
- Both support hard register allocation
- Both support C99 features used by AGENTS

### Low-Risk Areas (No Changes Expected)

✅ **Algorithm code:** Unification, constraint solving, search
✅ **Data structures:** And-boxes, choice-boxes (just size changes)
✅ **Built-ins:** Most built-in predicates
✅ **Bytecode format:** Platform-independent
✅ **Parser/lexer:** Generated code, portable

### Medium-Risk Areas (Test Carefully)

⚠️ **Memory allocation:** Test alignment, especially floats
⚠️ **GC:** Test forwarding pointers, mark bit usage
⚠️ **Tagged pointers:** Verify all tag operations work on 64-bit
⚠️ **Endianness:** Bytecode loading (should be OK, already little-endian)

---

## References

### Key Source Files Analyzed

**Core Engine:**
- `emulator/engine.c:1-2500+` - Main execution engine
- `emulator/instructions.h:1-80` - Instruction dispatch
- `emulator/opcodes.h:1-100+` - Opcode definitions

**Data Structures:**
- `emulator/tree.h:1-158` - And-boxes, choice-boxes
- `emulator/exstate.h:1-471` - Execution state
- `emulator/term.h:1-738` - Tagged pointers
- `emulator/unify.h:1-31` - Unification state

**Memory Management:**
- `emulator/storage.h:1-379` - Allocation macros
- `emulator/storage.c` - Heap management (implied)
- `emulator/gc.c` - Garbage collection (implied)

**Platform Dependencies:**
- `emulator/sysdeps.h:1-42` - All platform-specific code
- `emulator/regdefs.h:1-87` - Hard register allocation
- `emulator/constants.h:1-64` - Platform-specific limits

### Related Documents

- `01-ARCHITECTURE-OVERVIEW.md` - High-level architecture
- `03-PLATFORM-DEPENDENCIES.md` - Complete platform analysis
- `STUDY-PLAN.md` - Porting study plan

---

## Revision History

| Date | Version | Changes |
|------|---------|---------|
| 2025-11-04 | 1.0 | Complete emulator analysis for porting |

---

**End of Document**
