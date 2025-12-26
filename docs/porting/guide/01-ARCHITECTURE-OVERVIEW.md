# AGENTS v0.9 - System Architecture Overview

**Document:** Phase 1, Document 1 of 10
**Status:** Complete
**Date:** 2025-11-04
**Purpose:** High-level system architecture and execution model analysis

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [System Overview](#system-overview)
3. [Architecture Components](#architecture-components)
4. [Execution Model](#execution-model)
5. [Data Representation](#data-representation)
6. [Virtual Machine Design](#virtual-machine-design)
7. [Memory Architecture](#memory-architecture)
8. [Concurrency Model](#concurrency-model)
9. [Porting Implications](#porting-implications)
10. [References](#references)

---

## Executive Summary

AGENTS v0.9 is a concurrent constraint programming system implementing the **Agent Kernel Language (AKL)**. The system uses a three-layer architecture:

1. **High-Level Layer:** AKL source code (.akl files)
2. **Intermediate Layer:** Bytecode representation (.pam files)
3. **Execution Layer:** C-based abstract machine emulator

The emulator implements a **WAM-inspired virtual machine** with extensions for concurrent constraint programming, agent-based computation, and port-based communication. The execution model supports:

- Concurrent agent execution with and-parallelism and or-parallelism
- Constraint solving (rational trees, finite domains)
- Nondeterministic search with choice points
- Dynamic predicate definition and higher-order constructs

### Key Architectural Characteristics

- **Word Size:** Currently 32-bit (TADBITS=32), with 64-bit support for Alpha
- **Tagged Pointers:** Primary and secondary tags in low bits for type discrimination
- **Garbage Collection:** Copying collector with flip-flop heaps
- **Bytecode Dispatch:** Threaded code (computed goto) or switch-based
- **Register Allocation:** Optional hard register mapping for performance

### Porting Effort Summary

**Target Architectures:** x86-64 (Intel/AMD) and ARM64 (ARMv8+)

**Estimated Effort:** 280-560 hours (8-14 weeks) across 4 implementation phases (see **09-IMPLEMENTATION-ROADMAP.md**)

**Overall Risk:** MEDIUM-HIGH due to system age and architecture transition (see **08-RISK-ASSESSMENT.md**)

**Key Success Factor:** The existing **Alpha 64-bit port** provides a proven blueprint, significantly reducing porting risk.

**Platform Compatibility:** See **10-COMPATIBILITY-MATRIX.md** for complete platform comparison and expected performance.

---

## System Overview

### What is AGENTS?

From README:1-31:
> AGENTS is a system for programming in AKL. It provides a complete implementation of AKL with equality constraints over rational trees, various "finite domain" constraints over integers, and ports, a novelty of AKL. It also provides assorted built-in agents and other support.

### Historical Context

- **Origin:** Swedish Institute of Computer Science (SICS)
- **Version:** 0.9 (as indicated by README and sources)
- **Development Team:** Johan Bevemyr, Sverker Janson, Johan Montelius, and others
- **Current Target:** SPARC (primary), with GMP supporting 20+ architectures

### Programming Paradigm

AKL is a **concurrent constraint programming language** where:
- Computation is performed by **agents** interacting through **constraint stores**
- Agents may be thought of as processes, objects, functions, relations, or constraints
- Communication happens via **ports** (a novel AKL feature)
- Constraints include equality over rational trees and finite domain constraints

---

## Architecture Components

### Component Hierarchy

```
┌─────────────────────────────────────────────────────────────┐
│                    AKL Source Code (.akl)                   │
│                   User Programs & Libraries                  │
└────────────────────────────┬────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────┐
│                  AKL Compiler (compiler/)                   │
│              Written in AKL, ~30+ modules                   │
│           Produces PAM bytecode from AKL source             │
└────────────────────────────┬────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────┐
│                Bytecode Format (.pam files)                 │
│           Portable Abstract Machine Instructions            │
└────────────────────────────┬────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────┐
│                  Emulator (emulator/ - C)                   │
│  ┌───────────────────────────────────────────────────────┐  │
│  │          Bytecode Interpreter (engine.c)              │  │
│  │    • Threaded code dispatch or switch-based           │  │
│  │    • ~115 instruction opcodes                        │  │
│  │    • Register-based VM with continuation passing      │  │
│  ├───────────────────────────────────────────────────────┤  │
│  │      Memory Manager (storage.c, gc.c)                 │  │
│  │    • Heap allocation with block management            │  │
│  │    • Copying garbage collector (flip-flop)            │  │
│  │    • Constant space for atoms/functors                │  │
│  ├───────────────────────────────────────────────────────┤  │
│  │       Unification Engine (unify.c)                    │  │
│  │    • Rational tree unification                        │  │
│  │    • Occurs check                                     │  │
│  │    • Constraint propagation hooks                     │  │
│  ├───────────────────────────────────────────────────────┤  │
│  │     Constraint Solvers (fd.c, fd_akl.c)               │  │
│  │    • Finite domain constraints                        │  │
│  │    • Suspension and waking mechanisms                 │  │
│  ├───────────────────────────────────────────────────────┤  │
│  │    Agent Execution (candidate.c, exstate.c)           │  │
│  │    • And-box and choice-box management                │  │
│  │    • Agent scheduling and context switching           │  │
│  │    • Trail for backtracking                           │  │
│  ├───────────────────────────────────────────────────────┤  │
│  │         Built-in Predicates (builtin.c)               │  │
│  │    • I/O, arithmetic, reflection, ports               │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────┬───────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────┐
│              GMP Library (gmp/ - C + ASM)                   │
│        Arbitrary-precision arithmetic (excluded)             │
└─────────────────────────────────────────────────────────────┘
```

### Directory Structure with Sizes

From initial analysis:

| Directory | Size | Purpose | Files |
|-----------|------|---------|-------|
| emulator/ | 3.3M | Core VM in C | 50 .c, 63 .h |
| gmp/ | 14M | Bignum library | Excluded from study |
| compiler/ | 614K | Bytecode compiler | ~30 .akl modules |
| environment/ | 546K | Built-in agents | .akl files |
| library/ | 3.7M | User libraries | Package modules |
| demos/ | 2.4M | Example programs | 85+ .akl files |
| tests/ | 59K | DejaGnu tests | Test suite |
| tools/ | 126K | SAGA tools | Utility packages |
| doc/ | 1.1M | Documentation | LaTeX, info |

---

## Execution Model

### Virtual Machine Type

The AGENTS emulator implements a **register-based abstract machine** inspired by the **Warren Abstract Machine (WAM)** but extended for concurrent constraint programming.

### Key Execution Characteristics

From emulator/engine.c:106-154:

```c
void tengine(exs, initcodes)
     exstate *exs;
     int initcodes;
{
  register code   *pc REGISTER3;     /* The code pointer */
  register opcode op REGISTER4;      /* The next opcode */
  register Term   *areg REGISTER5;   /* The argument registers */
  register Term   *yreg REGISTER6;   /* Current environment */
  register andbox *andb REGISTER2;   /* The current and box */
  register choicebox *chb;           /* The current choice box */
```

**Architecture Features:**

1. **Register-Based Design**
   - Argument registers (X registers): areg[0..MAX_AREGS-1]
   - Environment registers (Y registers): yreg[0..N]
   - MAX_AREGS = 256 (typical) or 8192 (large config)

2. **Hard Register Allocation (Optional)**
   - Emulator variables can be mapped to physical CPU registers
   - Controlled by `-DHARDREGS` compiler flag
   - Significant performance benefit on register-rich architectures

3. **Instruction Dispatch**
   - **Threaded Code:** Uses gcc's computed goto (&&label syntax)
   - **Switch-Based:** Fallback for non-gcc compilers
   - Performance critical: dispatches ~115 instruction types

### Execution State

From emulator/exstate.h:417-442:

```c
typedef struct exstate {
  struct exstate   *father;       /* Parent execution state */
  struct exstate   *next;         /* Sibling states */
  struct exstate   *child;        /* Child states */
  code             *pc;           /* Program counter */
  uword            arity;         /* Current arity */
  predicate        *def;          /* Predicate being executed */
  choicebox        *insert;       /* Choice insertion point */
  andbox           *andb;         /* Current and-box */
  choicebox        *root;         /* Root choice-box */
  trailhead        trail;         /* Backtracking trail */
  taskhead         task;          /* Task queue */
  recallhead       recall;        /* Recall stack */
  wakehead         wake;          /* Wake queue */
  contexthead      context;       /* Context stack */
  suspendhead      suspend;       /* Suspension stack */
  bool             tracing;       /* Debug mode */
  bool             debugging;     /* Debug mode */
  struct unifystate urec;         /* Unification state */
  Term             areg[MAX_AREGS]; /* Argument registers */
} exstate;
```

**Execution state contains:**
- Program counter and registers
- And-box and choice-box management
- Trail for backtracking
- Task, wake, and recall queues for concurrency
- Suspension mechanism for constraints

---

## Data Representation

### Tagged Pointer Scheme

From emulator/term.h:3-22:

The system uses a **sophisticated tagged pointer scheme** with primary and secondary tags in low bits:

```
32-bit word layout:

Reference:                    0:::::::::::::::::::::::::::::00
Unbound unconstrained var:    0:::::::::::::::::::::::::::::01
Unbound constrained var:      0:::::::::::::::::::::::::::::11
List cell:                    0:::::::::::::::::::::::::::0010
Structure:                    0:::::::::::::::::::::::::::1010
Generic/float/bignum:         0:::::::::::::::::::::::::::1110
Small integer:                0::::::::::::::::::::::::::00110
Atom:                         0::::::::::::::::::::::::::10110

High bit (bit 31): GC mark bit
Low 2 bits: Primary tag
Low 4 bits: Secondary tag (for IMM, LST, STR, GEN)
```

### Type Definitions

From emulator/term.h:59-152:

```c
typedef unsigned long uword;
typedef long sword;
typedef uword tagged_address;

#define TADBITS 32              /* Word size in bits */
#define WORDALIGNMENT (TADBITS/8)  /* 4 bytes on 32-bit */

#define PTagMask  ((uword)0x00000003)  /* Primary tag: 2 bits */
#define STagMask  ((uword)0x0000000f)  /* Secondary tag: 4 bits */
#define IMM_ATM_BIT ((uword)0x10)      /* Atom bit in immediates */
#define SMALLNUMBITS (TADBITS-6)       /* 26 bits for small ints */

typedef enum {
    REF = 0,      /* Reference (pointer to variable) */
    UVA = 1,      /* Unbound unconstrained variable */
    GVA = 3,      /* Unbound constrained variable */
    IMM = 6,      /* Immediate (atom or small int) */
    LST = 2,      /* List cell */
    STR = 0xa,    /* Structure */
    GEN = 0xe     /* Generic object */
} tval;
```

**Key Implications:**
- **Double-word alignment required** for structures/lists to keep low 2 bits clear
- **4-bit tags** require downshifting by 2 bits before pointer use
- **Small integers:** 26 bits signed (-33554432 to +33554431)
- **High bit reserved** for GC marking

### 64-bit Considerations

From emulator/sysdeps.h:1-3:

```c
#ifdef __alpha__
#define TADBITS 64
#endif
```

**Already has 64-bit support for Alpha!** This suggests the tagging scheme is designed to scale to 64 bits.

On 64-bit:
- Small integers would be 58 bits signed
- More address space for heap
- Pointer alignment may differ
- GC mark bit in bit 63

---

## Virtual Machine Design

### Instruction Set Architecture

From emulator/opcodes.h:1-100:

The VM has ~115 instruction opcodes organized into categories:

1. **Control Flow**
   - SWITCH_ON_TERM, TRY, RETRY, TRUST
   - CALL, EXECUTE, PROCEED
   - GUARD_* instructions for concurrent guards

2. **Register Operations**
   - GET_X_VARIABLE, GET_Y_VARIABLE
   - PUT_X_VALUE, PUT_Y_VALUE
   - UNIFY_* instructions

3. **Data Construction**
   - GET_STRUCTURE, PUT_STRUCTURE
   - GET_LIST, PUT_LIST
   - GET_CONSTANT, PUT_CONSTANT

4. **Indexing**
   - SWITCH_ON_CONSTANT
   - SWITCH_ON_STRUCTURE

5. **Concurrency (AKL-specific)**
   - GUARD_UNIT, GUARD_ORDER, GUARD_UNORDER
   - SUSPEND_FLAT
   - SEND3 (port communication)

6. **BAM Extensions** (optional)
   - LOAD_*, READ_*, WRITE_* instructions
   - Specialized constraint instructions

### Threaded Code Implementation

From emulator/engine.c:49-66, 155-165:

```c
#ifdef THREADED_CODE
address *instr_label_table = NULL;
long instr_label_table_length = 0;

static address label_table[] = {
#include "opcodes.h"  /* Expands to: &&CaseLabel(OPCODE), ... */
};
#endif
```

**Threaded code uses gcc's label-as-values extension:**
- Jump table with addresses of instruction handlers
- Direct jump to next instruction (no switch overhead)
- Requires gcc 2.0+ with computed goto support
- Critical for performance (~20-30% speedup vs switch)

**Dispatch macro:**
```c
#define NextOp()  goto *instr_label_table[*pc++]
```

**Switch fallback:**
```c
#define NextOp()  break;  /* Returns to top of switch */
```

### Choice Points and Backtracking

The system implements **or-parallelism** via choice-boxes:

From emulator/engine.c:260-370:

- **TRY:** Create choice-box for first clause
- **RETRY:** Try next clause, restoring argument registers
- **TRUST:** Last clause, remove choice continuation
- **TRY_SINGLE:** Special case for deterministic single clause

**Trail mechanism** records variable bindings for undo on backtracking.

### And-Boxes for Concurrent Agents

The system implements **and-parallelism** via and-boxes:

- **And-box:** Represents a conjunction of goals (agent body)
- **Choice-box:** Represents a disjunction (alternative clauses)
- **Agent scheduling:** Multiple agents can execute concurrently
- **Suspension:** Agents can suspend on constraint variables

---

## Memory Architecture

### Heap Organization

From emulator/storage.h:320-379:

```c
typedef char heap;

typedef struct block {
  flipstate flipflop;      /* For GC: FLIP or FLOP */
  struct block *next;      /* Linked list of blocks */
  heap  *start;            /* Start of block */
  heap  *end;              /* End of block */
  heap  *logical_end;      /* Allocation boundary */
} block;

extern heap  *glob_heapcurrent;  /* Current allocation pointer */
extern heap  *glob_heapend;      /* End of current block */
```

**Heap allocation:**
- **Block-based:** Heap divided into blocks linked together
- **Bump-pointer allocation:** Fast allocation via pointer increment
- **Overflow handling:** New blocks allocated when current is full

### Memory Spaces

From emulator/storage.h:354-360:

```c
extern heap  *constspace;        /* Constant area start */
extern heap  *constend;          /* Constant area end */
extern heap  *constcurrent;      /* Constant allocation pointer */

extern int   heapsize;           /* Configurable heap size */
extern int   blocksize;          /* Configurable block size */
extern int   constsize;          /* Configurable constant size */
```

**Three memory spaces:**

1. **Heap Space:** Dynamic allocations (terms, structures, lists)
2. **Constant Space:** Atoms and functors (not garbage collected)
3. **Stack Spaces:** Trail, task queue, context stack, etc.

### Alignment Requirements

From emulator/term.h:78-80:

```c
#ifndef WORDALIGNMENT
#define WORDALIGNMENT  (TADBITS/8)
#endif
```

From emulator/storage.h:5-9:

```c
#define WordAlign(x) (((x) + (WORDALIGNMENT-1)) & ~((uword)(WORDALIGNMENT-1)))

#ifndef OptionalWordAlign
#define OptionalWordAlign(x)  x
#endif
```

**Alignment rules:**
- **Required:** WORDALIGNMENT bytes (4 on 32-bit, 8 on 64-bit)
- **Optional:** Some platforms require stricter alignment (Macintosh)
- **Floating-point:** Aligned to sizeof(double) for performance

### Garbage Collection

From emulator/storage.h:327-345:

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

**Copying GC with flip-flop heaps:**
- **Two-space algorithm:** Flip and flop spaces alternate
- **Mark bit:** High bit in tagged pointers used for forwarding
- **Scavenging:** Depth-first copying with mark-sweep
- **Close list:** Special handling for generic objects with external resources

---

## Concurrency Model

### Agent-Based Execution

AGENTS implements **concurrent constraint programming** with:

1. **And-Parallelism:** Multiple agents in a conjunction execute concurrently
2. **Or-Parallelism:** Multiple clauses tried as alternatives (choice points)
3. **Guard-Based Selection:** Guards control which clause executes
4. **Port-Based Communication:** Agents communicate via ports (message passing)

### Guard Semantics

From emulator/engine.c:374-487:

Three types of guards:

1. **GUARD_UNIT:** Leftmost guard succeeds, promotes immediately
2. **GUARD_ORDER:** Ordered guard, must be leftmost to commit
3. **GUARD_UNORDER:** Unordered guard, can commit when solved

**Promotion:** When a guard succeeds, its and-box is promoted to parent context

### Constraint Suspension

From emulator/exstate.h:39-47:

```c
#define MakeBigVar(R)\
{\
    svainfo *new;\
    NewSvainfo(new);\
    new->method = NULL;\
    new->env = Env(RefTerm(R));\
    new->susp = NULL;\
    RefTerm(R) = TagGva(new);  /* Upgrade to constrained variable */
}
```

**Suspension mechanism:**
- Variables can have **suspension lists**
- When bound, suspended agents are **woken**
- Supports finite domain constraints and port operations

---

## Porting Implications

### Architecture Dependencies Summary

From analysis of emulator/sysdeps.h and emulator/regdefs.h:

#### 1. Word Size Dependencies

**Current:**
- TADBITS = 32 (default)
- TADBITS = 64 (Alpha)

**Impact on x86-64/ARM64:**
- Must set TADBITS = 64
- Tagged pointer scheme should work (already has Alpha support)
- Small integer range increases from 26 to 58 bits
- WORDALIGNMENT becomes 8 bytes

#### 2. Pointer Origin (PTR_ORG)

From emulator/sysdeps.h:17-20, 38-40:

```c
#ifdef hppa
#  define PTR_ORG  ((uword)0x40000000)
#endif

#ifdef _AIX
#define PTR_ORG   ((uword)0x20000000)
#endif
```

**Purpose:** Subtract base address to avoid tag conflicts with high address bits

**Modern platforms:**
- x86-64: User space typically 0x0000000000000000 - 0x00007fffffffffff
- ARM64: Similar, varies by OS
- **Likely PTR_ORG = 0 will work** (low bits always available)

#### 3. Endianness

**Current design:** Little-endian assumed in some bit manipulations

**Modern targets:**
- x86-64: Little-endian (compatible)
- ARM64: Little-endian mode typical (bi-endian capable)
- **Should be compatible with minimal changes**

#### 4. Alignment

From emulator/sysdeps.h:28-30:

```c
#ifdef macintosh
#define OptionalWordAlign(x)  WordAlign((unsigned long)(x))
#endif
```

**Modern platforms:**
- x86-64: Relaxed alignment (unaligned access permitted, may be slower)
- ARM64: Strict alignment preferred (unaligned access may fault)
- **May need OptionalWordAlign for ARM64**

#### 5. Hard Register Allocation

From emulator/regdefs.h:1-45:

Currently defined for:
- MIPS: $14-$23 (10 registers)
- SPARC: %l2-l7, %i3-i5 (9 registers)
- Alpha: $14-$23 (10 registers)

**Modern platforms:**
- x86-64: 16 GPRs (rax-r15), can allocate r12-r15, rbx, rbp
- ARM64: 31 GPRs (x0-x30), can allocate x19-x28 (callee-saved)

**Recommendation:** Add register definitions for x86-64 and ARM64

#### 6. Computed Goto Support

**Requirement:** gcc 2.0+ or clang (both support &&label syntax)

**Modern platforms:**
- x86-64: Fully supported (gcc, clang)
- ARM64: Fully supported (gcc, clang)
- **No compatibility issues**

### Portability Assessment

**High Confidence Areas:**
- ✅ Threaded code dispatch (gcc/clang support)
- ✅ Tagged pointer scheme (already has 64-bit support)
- ✅ Endianness (little-endian compatible)
- ✅ Compiler features (gcc/clang have all required features)

**Medium Confidence Areas:**
- ⚠️  Memory alignment (may need ARM64 tuning)
- ⚠️  Hard registers (need new definitions, optional feature)
- ⚠️  PTR_ORG value (likely 0, needs testing)

**Low Risk Areas:**
- ✅ C language features (ANSI C with gcc extensions)
- ✅ No inline assembly in emulator core (all in GMP)

---

## References

### Source Files Analyzed

- `emulator/term.h:1-738` - Tagged pointer scheme and data types
- `emulator/engine.h:1-24` - Engine interface
- `emulator/engine.c:1-500` - Main execution engine (partial)
- `emulator/bam.h:1-191` - BAM extensions
- `emulator/worker.h:1-262` - Worker (old?) execution state
- `emulator/storage.h:1-379` - Memory management
- `emulator/exstate.h:1-471` - Execution state structures
- `emulator/sysdeps.h:1-42` - Platform dependencies
- `emulator/regdefs.h:1-87` - Register allocation definitions
- `emulator/opcodes.h:1-100` - Instruction opcodes (partial)
- `README:1-231` - System overview and installation

### Related Documents

**Phase 1: Architecture** (Documents 1-3)
- `02-EMULATOR-ANALYSIS.md` - Detailed emulator code analysis
- `03-PLATFORM-DEPENDENCIES.md` - Complete platform dependency inventory

**Phase 2-4: Technical Analysis** (Documents 4-7)
- `04-MEMORY-MANAGEMENT.md` - Memory model, GC, and alignment
- `05-BYTECODE-DISPATCH.md` - Instruction set and dispatch mechanisms
- `06-BUILD-SYSTEM.md` - Build system modernization requirements
- `07-TESTING-STRATEGY.md` - Testing and validation framework

**Phase 5: Planning** (Documents 8-10)
- `08-RISK-ASSESSMENT.md` - Risk analysis and mitigation strategies
- `09-IMPLEMENTATION-ROADMAP.md` - Detailed 4-phase implementation plan
- `10-COMPATIBILITY-MATRIX.md` - Platform compatibility and feature matrix

**Overall**
- `STUDY-PLAN.md` - Porting study plan and status

---

## Revision History

| Date | Version | Changes |
|------|---------|---------|
| 2025-11-04 | 1.0 | Initial document created for Phase 1 |

---

**End of Document**
