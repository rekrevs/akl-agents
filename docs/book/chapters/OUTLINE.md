# Inside AGENTS: A Concurrent Constraint Programming System

## Book Outline - First Draft

**Purpose:** Comprehensive documentation of the AGENTS/AKL system's design, architecture, and implementation.

**Target Audience:** Implementers, researchers, and advanced users interested in the internal workings of concurrent constraint programming systems and abstract machine design.

---

## Part I: Foundations

### Chapter 1: Introduction
**Purpose:** Set the stage—what AGENTS is, why it exists, its context in the history of constraint and logic programming.

**Content:**
- What is AGENTS?
- Historical context: SICS, concurrent constraint programming
- The Andorra Kernel Language (AKL) paradigm
- Relationship to Prolog, WAM, and concurrent logic languages
- Key innovations: guards, ports, agent-based computation
- Reading guide and chapter overview

### Chapter 2: The AKL Language
**Purpose:** Describe the programming language that AGENTS implements—just enough to understand the implementation.

**Content:**
- Syntax and basic constructs
- Horn clauses vs. guarded clauses
- Concurrent agents and the agent abstraction
- Constraint stores and constraint solving
- Port-based communication
- Nondeterminism: don't-care vs. don't-know
- Simple examples demonstrating key features
- Differences from Prolog

---

## Part II: System Architecture

### Chapter 3: Overall Architecture
**Purpose:** The big picture—how the pieces fit together.

**Content:**
- Three-layer architecture
  - AKL source code (.akl)
  - PAM bytecode (.pam)
  - C-based emulator
- Compiler pipeline
- Runtime system organization
- Directory structure and components
- Build system and platform support
- External dependencies (excluding GMP/FD)

### Chapter 4: The Prototype Abstract Machine (PAM)
**Purpose:** Describe the abstract machine instruction set and execution model.

**Content:**
- Relationship to Warren Abstract Machine (WAM)
- Extensions for concurrency and constraints
- Instruction set categories
  - Control flow (CALL, EXECUTE, PROCEED)
  - Register operations (GET/PUT X/Y variables)
  - Data construction (structures, lists, atoms)
  - Indexing (SWITCH_ON_TERM, etc.)
  - Concurrency primitives (GUARD_*, SUSPEND)
  - Port operations (SEND3)
- Bytecode format (.pam files)
- Code loading and linking

### Chapter 5: Execution Model
**Purpose:** How programs execute at a conceptual level.

**Content:**
- Register-based virtual machine
- Execution state structure
- And-boxes and or-parallelism (choice-boxes)
- Agent scheduling and context switching
- Guards and their semantics
  - GUARD_UNIT, GUARD_ORDER, GUARD_UNORDER
  - Promotion and commitment
- Suspension and waking
- Trail for backtracking
- Task queues and execution flow

---

## Part III: Core Implementation

### Chapter 6: Data Representation
**Purpose:** How data is stored and manipulated in memory.

**Content:**
- Tagged pointer scheme
  - Primary tags (2 bits)
  - Secondary tags (4 bits)
  - Type encoding: REF, UVA, GVA, IMM, LST, STR, GEN
- Word size considerations (32-bit vs 64-bit)
- Small integers vs. bignums
- Atoms and functors
- Lists and structures
- Variables: unbound, bound, constrained
- Generic objects
- The term type hierarchy

### Chapter 7: Memory Management
**Purpose:** How memory is allocated, organized, and reclaimed.

**Content:**
- Memory spaces
  - Heap space (dynamic terms)
  - Constant space (atoms/functors)
  - Stack spaces (trail, contexts, etc.)
- Block-based heap organization
- Bump-pointer allocation
- Alignment requirements
  - WORDALIGNMENT macro
  - Platform-specific alignment (x86-64 vs ARM64)
- Memory growth and overflow handling
- The PTR_ORG concept

### Chapter 8: Garbage Collection
**Purpose:** The copying collector and its mechanisms.

**Content:**
- Two-space flip-flop algorithm
- Mark bit in high bit of tagged pointers
- Forwarding pointers
- Scavenging depth-first
- Roots: execution state, choice-boxes, and-boxes
- Close list for generic objects
- GC triggering and heap sizing
- Performance characteristics

### Chapter 9: The Execution Engine
**Purpose:** The heart of the system—the bytecode interpreter.

**Content:**
- engine.c: structure and organization
- The tengine() function
- Register allocation strategy
  - Soft registers (C variables)
  - Hard registers (physical CPU registers)
  - REGISTER1-6 macros
- Main execution loop
- Instruction dispatch: threaded code vs. switch
- State saving and restoring
- Exception handling (GC, interrupts)

### Chapter 10: Instruction Dispatch
**Purpose:** How instructions are decoded and executed efficiently.

**Content:**
- Threaded code implementation
  - Computed goto (&&label syntax)
  - Label table construction
  - Jump table dispatch
- Switch-based fallback
- Performance comparison
- Compiler support requirements (gcc/clang)
- Platform differences

---

## Part IV: Advanced Features

### Chapter 11: Unification
**Purpose:** The core operation for constraint solving.

**Content:**
- Rational tree unification
- Occurs check
- Unification modes (read vs. write)
- Structure unification
- List unification
- Dereferencing
- Constraint propagation hooks
- Performance optimizations

### Chapter 12: Choice Points and Backtracking
**Purpose:** Implementing nondeterministic search.

**Content:**
- Choice-box structure
- TRY/RETRY/TRUST instructions
- Argument register saving
- Trail mechanism
- Backtracking protocol
- Or-parallelism conceptual model
- Interaction with promotion

### Chapter 13: And-Boxes and Concurrency
**Purpose:** The and-parallel execution model.

**Content:**
- And-box structure
- Agent bodies as and-boxes
- Father/child relationships
- Sibling execution states
- Candidate selection for choice splitting
- Suspension lists
- Context switching
- Concurrent agent scheduling

### Chapter 14: Constraint Variables and Suspension
**Purpose:** How constraints integrate with execution.

**Content:**
- Unbound unconstrained variables (UVA)
- Unbound constrained variables (GVA)
- svainfo structure
- Suspension lists on variables
- Wake queues
- Constraint propagation
- Integration with guards

### Chapter 15: Port Communication
**Purpose:** Message passing between agents.

**Content:**
- Port abstraction
- SEND3 instruction
- Port implementation
- Buffering and delivery
- Integration with suspension
- Use cases and patterns

---

## Part V: Support Systems

### Chapter 16: Built-in Predicates
**Purpose:** The primitive operations available to programs.

**Content:**
- builtin.c organization
- Arithmetic operations
- I/O operations
- Reflection and meta-programming
- Type checking
- Port operations
- System predicates
- Foreign function interface

### Chapter 17: Compiler
**Purpose:** From AKL source to PAM bytecode.

**Content:**
- Compiler architecture (written in AKL)
- Parsing AKL syntax
- Translation to PAM
- Optimizations
- Code generation
- Predicate indexing
- Compilation modes (compile vs. compilef)

### Chapter 18: Debugging and Tracing
**Purpose:** Tools for understanding program behavior.

**Content:**
- Four-port debugger model
- Extended ports for guards (suspend, wake)
- Trace instructions
- Spy points
- Debugger commands
- Implementation in trace.c
- Performance impact

---

## Part VI: Platform Support

### Chapter 19: Platform Dependencies
**Purpose:** What varies across architectures and how it's handled.

**Content:**
- sysdeps.h: platform detection
- Word size: TADBITS
- Pointer origin: PTR_ORG
- Alignment: WORDALIGNMENT, OptionalWordAlign
- Endianness considerations
- Platform-specific code paths

### Chapter 20: Register Allocation
**Purpose:** Mapping emulator variables to physical registers.

**Content:**
- regdefs.h: register definitions
- Per-platform register allocation
  - Alpha (64-bit reference)
  - SPARC
  - MIPS
  - x86-64
  - ARM64
- Callee-saved vs. caller-saved registers
- Performance impact of HARDREGS
- Portability considerations

### Chapter 21: Build System
**Purpose:** How the system is configured and compiled.

**Content:**
- autoconf-based configuration
- configure.in and configure
- Platform detection
- Compiler feature detection
- Makefile structure
- Build targets
- Installation

### Chapter 22: 64-bit Porting
**Purpose:** The transition to modern 64-bit platforms.

**Content:**
- Why 64-bit? (modern platform requirements)
- The Alpha 64-bit port as blueprint
- Tagged pointer scheme scaling
- Small integer expansion (26-bit to 58-bit)
- Memory alignment changes
- x86-64 specific considerations
- ARM64 specific considerations
- Lessons learned and best practices

---

## Part VII: Testing and Evolution

### Chapter 23: Test Suite
**Purpose:** How correctness is validated.

**Content:**
- DejaGnu test framework
- Test organization in tests/
- Demo programs as tests
- Benchmarks
- Regression testing
- Platform-specific tests

### Chapter 24: Performance Characteristics
**Purpose:** Understanding system performance.

**Content:**
- Benchmarking methodology
- Instruction dispatch overhead
- Memory allocation costs
- GC pause times
- Threading overhead
- Hard register benefits
- Platform comparisons

### Chapter 25: Design Rationale and Tradeoffs
**Purpose:** Why things are the way they are.

**Content:**
- Heap vs. stack allocation
- Copying vs. sharing for nondeterminism
- Threaded code vs. switch
- Block-based heap
- Two-space GC
- And-box structure
- Historical evolution
- Alternative designs considered

---

## Part VIII: Appendices

### Appendix A: Instruction Reference
**Complete reference for all ~115 PAM instructions**

### Appendix B: Data Structure Reference
**Key C structures with field descriptions**

### Appendix C: Configuration Options
**All compile-time flags and their effects**
- THREADED_CODE
- HARDREGS
- BAM
- TRACE
- STRUCT_ENV
- INDIRECTVARS
- TADTURMS

### Appendix D: File Organization
**Complete source file inventory with descriptions**

### Appendix E: Glossary
**Terms and concepts**

### Appendix F: Bibliography
**Papers, references, related work**

---

## Writing Approach

For each chapter:
1. **Start with a summary paragraph** describing what this chapter covers and why it matters
2. **Provide necessary background** from previous chapters
3. **Present concepts incrementally** from simple to complex
4. **Use code examples** with line number references
5. **Include diagrams** where helpful (described in text)
6. **End with key takeaways** and connections to other chapters

For each section within chapters:
1. **State the purpose** of this section
2. **Present the information** with concrete details
3. **Provide examples** from actual source code
4. **Explain implications** for implementers

## Sources

Primary sources for reconstruction:
- Existing documentation in docs/porting/
- Source code in emulator/
- README and STUDY-PLAN.md
- doc/internals.tex
- Demo programs showing usage
- Build system configuration

---

**End of Outline**
