# Chapter 1: Introduction

## What is AGENTS?

AGENTS is a complete implementation of the **Andorra Kernel Language (AKL)**, a concurrent constraint programming language developed at the Swedish Institute of Computer Science (SICS) in the early 1990s. At its core, AGENTS represents an ambitious attempt to unify multiple programming paradigms—logic programming, concurrent programming, constraint solving, and functional programming—into a single coherent computational model based on the concept of **agents** interacting through **constraint stores**.

The system provides:

- A **compiler** that translates AKL source code to an abstract machine bytecode
- An **emulator** implementing a register-based virtual machine in C
- A **runtime system** with garbage collection, debugging, and extensive built-in predicates
- A **library** of reusable packages written in AKL
- Support for **finite domain constraints** and **port-based communication**

As of November 2025, AGENTS has been successfully ported to modern 64-bit platforms including x86-64 (Intel/AMD) and ARM64 (Apple Silicon/AArch64), ensuring its continued relevance on contemporary hardware.

## Historical Context

### The Andorra Principle

The name "Andorra" comes from the **Andorra Principle**, proposed by David H.D. Warren in 1987, which suggests that deterministic computation should proceed eagerly while nondeterministic choices should be delayed. This principle attempts to get the best of both worlds from logic programming: the expressiveness of nondeterministic search with the efficiency of deterministic execution.

### The Logic Programming Landscape

To understand AGENTS, we must situate it within the evolution of logic programming:

**First Generation: Prolog (1970s-1980s)**
- Horn clause logic programming
- Sequential execution with backtracking
- Don't-know nondeterminism
- Efficient but limited parallelism

**Concurrent Logic Languages (1980s)**
- GHC (Guarded Horn Clauses), Parlog, Concurrent Prolog
- Process-oriented computation
- Don't-care nondeterminism (committed choice)
- No backtracking, but natural concurrency

**Constraint Logic Programming (1980s-1990s)**
- CLP(R), CLP(FD), and others
- Integration of constraint solving with logic programming
- Declarative problem solving
- Constraint propagation and search

**AKL: The Synthesis (1990s)**

AKL emerged as a **general combination** of these approaches:
- Nondeterministic search (like Prolog)
- Concurrent agent execution (like GHC)
- Constraint solving (like CLP)
- Novel port-based communication
- Guard-based clause selection

The Andorra Kernel Language was designed to be a **kernel language**—a minimal, orthogonal set of primitives from which higher-level constructs could be built. It was intended both as a practical programming language and as a research vehicle for exploring the design space of concurrent constraint programming.

### The SICS Legacy

The Swedish Institute of Computer Science (SICS) has been a major center for logic programming research. Key contributors to AGENTS and AKL include:

- **Sverker Janson**: Principal author of the AGENTS system and its emulator
- **Johan Bevemyr**: Original emulator implementation, compiler adaptation
- **Johan Montelius**: Language design, runtime systems
- **Seif Haridi**: Language design, theoretical foundations
- **Per Brand**: Runtime system contributions
- **Peter Olin**: Original Prolog compiler that was adapted for AKL

The work on AKL and AGENTS was part of a broader research program at SICS on concurrent constraint programming, which also produced influential work on Oz and Mozart.

## The AKL Paradigm

### Agents as the Central Abstraction

In AKL, computation is performed by **agents**. An agent is a more general concept than a process or function:

- In the context of **Horn clause logic**, agents act as **relations** or **predicates**
- In the context of **concurrency**, agents act as **processes** executing in parallel
- In the context of **object-oriented programming**, agents can encapsulate state and behavior
- In the context of **functional programming**, agents can compute values
- In the context of **constraints**, agents can be constraints themselves

This polymorphic nature of agents makes AKL exceptionally expressive.

### Constraint Stores

Agents interact through **constraint stores**. A constraint store is a set of constraints that describes properties of variables. As computation proceeds:

1. Agents **tell** constraints to the store (adding information)
2. Agents **ask** questions about the store (querying information)
3. The store maintains **logical consistency** through constraint propagation
4. Variables become increasingly **constrained** until they are **bound** to values

This model cleanly separates **computation** (what agents do) from **communication** (how they share information through stores).

### Guarded Clauses

AKL extends Horn clauses with **guards**. A clause has the form:

```
Head :- Guard ? Body.
```

Where:
- **Head** matches the goal being called
- **Guard** is a test that must succeed for the clause to be chosen
- **Body** is executed if the guard succeeds

Guards are crucial for two reasons:

1. **Synchronization**: Guards can suspend on uninstantiated variables, waiting for more information
2. **Committed Choice**: Once a guard succeeds, the choice is committed (no backtracking to try other clauses)

The interaction between guards and backtracking gives AKL its power: within a guard, you can explore alternatives nondeterministically, but once the guard commits, the choice is final.

### Port-Based Communication

A novel feature of AKL is **ports**. Ports provide **asynchronous message passing**:

- A port is a stream (list) with a **write-protected tail**
- Agents can **send** messages to a port without blocking
- Agents can **read** from a port by unifying with the stream
- Ports enable **pipeline parallelism** and **dataflow** patterns

Ports distinguish AKL from earlier concurrent logic languages which relied primarily on shared logical variables for communication.

### Don't-Know and Don't-Care Nondeterminism

AKL supports both forms of nondeterminism:

**Don't-Know Nondeterminism** (Prolog-style):
- Multiple clauses for a predicate represent alternatives
- If one choice fails, backtrack and try another
- All solutions can be explored
- Uses choice points and backtracking

**Don't-Care Nondeterminism** (GHC-style):
- Guards select among alternatives
- Once a guard commits, that choice is final
- No backtracking across commitment
- More efficient, suitable for concurrent systems

The interplay between these two forms—don't-know within guards, don't-care across guard commitment—is a defining characteristic of AKL.

## Relationship to Other Systems

### Warren Abstract Machine (WAM)

AGENTS builds upon the **Warren Abstract Machine** (WAM), the standard abstract machine for Prolog implementation. From WAM, AGENTS inherits:

- Register-based architecture
- Argument registers (X registers) and environment registers (Y registers)
- Structure/list representation
- Unification algorithms
- Indexing mechanisms

But AGENTS **extends** WAM significantly:

- And-boxes for concurrent agent execution
- Choice-boxes for or-parallelism
- Guard instructions with suspension
- Port operations
- Constraint variable support
- Copying promotion for nondeterminism

Where WAM has a single execution state with a trail for backtracking, AGENTS has a **tree of execution states** representing concurrent and-parallel computation, with choice points providing or-parallelism.

### Prolog Systems

AGENTS can execute many Prolog programs with minimal modification:

```prolog
% Standard Prolog
ancestor(X,Y) :- parent(X,Y).
ancestor(X,Y) :- parent(X,Z), ancestor(Z,Y).
```

This works in AKL because the default translation of Horn clauses is:

```
H :- B.  =>  H :- true ? B.
```

A trivial guard `true` succeeds immediately, making the clause behave like standard Prolog.

However, AGENTS is **not** a Prolog system. Differences include:

- Different built-in predicates
- Different operational semantics for negation and cuts
- No database assertion/retraction (predicates are immutable)
- Ports instead of DCGs for I/O streams
- Different module system

### GHC and Concurrent Logic Languages

From concurrent logic languages like GHC (Guarded Horn Clauses), AKL adopts:

- Guard-based clause selection
- Committed choice semantics
- Process-oriented thinking
- Stream-based communication

But unlike pure GHC, AKL allows **backtracking within guards**, making it more expressive for search problems.

### Constraint Logic Programming

From CLP, AKL incorporates:

- Constraint solving over domains
- Suspension on uninstantiated variables
- Constraint propagation
- Declarative problem specification

AGENTS provides **finite domain (FD) constraints** over integers, enabling constraint-based search for combinatorial problems.

## Key Innovations

### 1. Unified Model

AKL's primary innovation is its **unification** of previously separate paradigms. The same program can exhibit:
- Sequential computation (like Prolog)
- Concurrent execution (like GHC)
- Constraint solving (like CLP)
- Dataflow parallelism (via ports)

This is not just syntactic sugar—these different styles genuinely coexist and compose.

### 2. Guards with Search

The ability to perform **nondeterministic search within guards** while still achieving **committed choice** is powerful. It means:
- Guards can try multiple strategies to succeed
- Once a guard succeeds, the commitment is efficient
- This enables sophisticated synchronization patterns

### 3. Ports as First-Class Objects

Making streams with write-protected tails into **first-class values** enables clean dataflow programming without the complexity of full unification in communication.

### 4. Explicit And-Parallelism

The **and-box** structure makes and-parallelism explicit in the implementation, enabling:
- Clear reasoning about concurrent execution
- Potential for parallel execution on multiprocessors
- Clean integration with or-parallelism

## System Architecture Overview

AGENTS follows a **three-layer architecture**:

```
┌─────────────────────────────────────┐
│   AKL Source Code (.akl files)     │  <- User programs
│   • Guards, ports, constraints      │
└──────────────┬──────────────────────┘
               │ compile
               ▼
┌─────────────────────────────────────┐
│   PAM Bytecode (.pam files)         │  <- Portable representation
│   • ~115 instruction opcodes        │
│   • Load-time linking               │
└──────────────┬──────────────────────┘
               │ load & execute
               ▼
┌─────────────────────────────────────┐
│   C Emulator (emulator/*.c)         │  <- Native execution
│   • Virtual machine                 │
│   • Garbage collection              │
│   • Built-in predicates             │
│   • Constraint solvers              │
└─────────────────────────────────────┘
```

**Layer 1: AKL Compiler** (~30 modules, written in AKL)
- Parses AKL syntax
- Performs simple optimizations
- Generates PAM bytecode
- Creates indexing structures

**Layer 2: PAM Bytecode** (portable binary format)
- Platform-independent
- Resembles WAM instructions with AKL extensions
- Can be saved to files or compiled in-memory
- Loaded dynamically by emulator

**Layer 3: Emulator** (~50 C files, ~15,000 lines)
- Interprets PAM instructions
- Manages memory with garbage collection
- Implements built-in operations
- Handles I/O, arithmetic, reflection
- Provides debugging and tracing

This separation of concerns enables:
- **Portability**: PAM bytecode runs on any platform with an emulator
- **Optimization**: Compiler optimizations are platform-independent
- **Maintainability**: Changes to the language affect only the compiler
- **Performance**: Critical inner loops are in hand-optimized C

## Implementation Highlights

### Register-Based Virtual Machine

The PAM emulator is a **register-based VM** (like modern VMs such as Dalvik/ART), not stack-based. This reduces instruction count for typical operations:

```
% AKL: f(X, Y) :- g(X), h(Y).

% PAM instructions (simplified):
CALL g/1         % X already in areg[0]
CALL h/1         % Y already in areg[0]
```

The emulator maintains:
- **Argument registers** (areg[0..255]): for passing arguments
- **Environment registers** (yreg[0..N]): for local variables
- **Program counter** (pc): current instruction
- **And-box pointer** (andb): current execution context
- **Choice-box pointer** (chb): current choice point

### Threaded Code Dispatch

For performance, the emulator uses **threaded code** (also called **direct threading**):

```c
// Jump table of instruction addresses
static void *dispatch_table[] = {
  &&LABEL_CALL,
  &&LABEL_EXECUTE,
  &&LABEL_TRY,
  // ... ~115 entries
};

// Dispatch macro
#define NextOp()  goto *dispatch_table[*pc++]

// Main loop
LABEL_CALL:
  // Execute CALL instruction
  // ...
  NextOp();  // Jump directly to next instruction's label
```

This is faster than a switch statement because it eliminates the branch to the top of the switch. Modern CPUs can predict these indirect branches well. This technique requires gcc or clang with the `&&label` extension.

### Tagged Pointers

All data is represented as **tagged pointers** fitting in a machine word:

```
64-bit word (on modern platforms):
┌──────────────────────────────────────────────────────┬─────────┬────┐
│ Payload (60 bits)                                    │ Sec Tag │ Pri│
└──────────────────────────────────────────────────────┴─────────┴────┘
 63                                                      3         1  0

Primary tags (2 bits):
  00 - Reference
  01 - Unbound variable
  10 - List or Structure (secondary tag distinguishes)
  11 - Generic or Immediate (secondary tag distinguishes)
```

This scheme is **self-describing**: every value carries its type. The tags use the low bits, which are always zero for aligned pointers, so no bit-shifting is needed for pointer dereferencing.

### Copying Garbage Collection

Memory management uses a **two-space copying collector**:

- Heap divided into two **semispaces** (flip and flop)
- Allocate in one space until full
- GC copies live data to other space
- Pointer forwarding uses the high bit as a mark
- Compact representation after collection

This simple algorithm ensures there are no memory leaks and keeps related data close together for cache efficiency.

### And-Boxes and Choice-Boxes

The execution model maintains a **tree structure**:

```
                   [Root Choice-Box]
                          |
              ┌───────────┼───────────┐
        [And-Box 1]  [And-Box 2]  [And-Box 3]
             |
        [Choice-Box]
             |
        [And-Box 1.1]
```

- **And-boxes** represent concurrent agents (conjunctions)
- **Choice-boxes** represent alternatives (disjunctions)
- This tree captures the full state of nondeterministic concurrent computation
- Backtracking walks this tree, trying different branches

### 64-bit Platform Support

Recent porting work leveraged an existing **Alpha 64-bit port** as a blueprint:

- Tagged pointer scheme scales naturally to 64-bit
- Small integers expand from 26 bits to 58 bits
- Memory alignment changes from 4 bytes to 8 bytes
- Register allocation definitions added for x86-64 and ARM64
- No GMP (arbitrary precision) support needed on modern platforms

The port was surprisingly straightforward due to careful abstraction in the original design.

## Reading Guide

This book is organized into seven parts:

**Part I: Foundations** (Chapters 1-2)
Start here for context. Understand what AGENTS is and the AKL language it implements.

**Part II: System Architecture** (Chapters 3-5)
The big picture: how compilation, bytecode, and execution fit together.

**Part III: Core Implementation** (Chapters 6-10)
The meat of the system: data representation, memory management, and the execution engine.

**Part IV: Advanced Features** (Chapters 11-15)
Specialized topics: unification, backtracking, concurrency, constraints, and ports.

**Part V: Support Systems** (Chapters 16-18)
Built-ins, compiler, and debugging tools.

**Part VI: Platform Support** (Chapters 19-22)
How AGENTS adapts to different architectures, including the 64-bit port.

**Part VII: Testing and Evolution** (Chapters 23-25)
Validation, performance, and design rationale.

**Part VIII: Appendices**
Quick reference material.

**For different audiences:**

- **Language designers**: Focus on Parts I-II and Part IV
- **VM implementers**: Focus on Parts III, IV, and VI
- **Compiler writers**: Focus on Parts II, V, and the compiler chapter
- **Platform porters**: Focus on Part VI and relevant chapters in Part III
- **Performance engineers**: Focus on Parts III, VI, and Chapter 24

**Reading paths:**

*Quick Overview*: Chapters 1, 3, 6, 9 give a condensed view of the system.

*Deep Dive*: Read linearly from Chapter 1 through Chapter 25 for complete understanding.

*Problem-Focused*: Use the index to find topics related to your specific question.

## Conventions

Throughout this book:

**Source references** use the format `filename:line` or `filename:function`:
- `engine.c:107` refers to line 107 of emulator/engine.c
- `storage.h:WordAlign` refers to the WordAlign definition

**Code examples** show actual code from the system:
```c
// From emulator/engine.c:117
register code *pc REGISTER3;
```

**AKL examples** show AKL language constructs:
```
ancestor(X,Y) :- parent(X,Y).
ancestor(X,Y) :- parent(X,Z), ancestor(Z,Y).
```

**Diagrams** are shown in ASCII art or described in text.

**File paths** are relative to the source root unless specified:
- `emulator/engine.c` means `$SRCDIR/emulator/engine.c`

**Version**: This book describes AGENTS v0.9 as of November 2025, including the 64-bit ports to x86-64 and ARM64.

## Chapter Preview

**Chapter 2** introduces the AKL language in enough detail to understand what the implementation must support. We'll see concrete examples of guards, ports, constraints, and concurrent computation.

**Chapter 3** zooms out to show how the compiler, bytecode format, and emulator work together as a system.

From there, we'll progressively dive deeper into the implementation, building understanding layer by layer.

---

**Next Chapter**: [Chapter 2: The AKL Language](chapter-02-akl-language.md)
