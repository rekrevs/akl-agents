# Inside AGENTS: Master Book Outline and Writing Plan

**Purpose:** This document provides the iterative writing plan for the entire book, following the principle of outlining structure, describing intent, then diving into subparts.

**Date:** 2025-11-06
**Status:** Planning phase

---

## Writing Methodology

### Iterative Approach

For each level (book → part → chapter → section):
1. **Outline structure** - What are the main components?
2. **Describe intent** - What should this section accomplish? (2-3 paragraphs)
3. **Break into subparts** - What are the subsections?
4. **Repeat recursively** - Apply same process to each subpart

### Source Material

**Available:**
- Complete AGENTS v0.9 source code
- Porting documentation (docs/porting/, docs/attempts/)
- Bug discovery narrative (docs/book/03-porting-story/discoveries.md)
- Build system analysis (docs/attempts/ATTEMPT-01/BUILD-SYSTEM-*.md)
- ARM64 porting docs (docs/book/05-platform-specifics/)

**To obtain:**
- Sverker Janson's thesis on AKL (mentioned but not yet available)
- Original SICS papers on AGENTS
- Related concurrent constraint programming research

**From code analysis:**
- Architecture and design patterns
- Memory management strategies
- Bytecode instruction set
- Garbage collection algorithm
- Tagged pointer implementation
- Register allocation strategy

---

## Book-Level Structure

### Part I: Introduction
**Intent:** Set context, explain what AGENTS is, why it matters, and what readers will learn.

**Audience considerations:**
- Systems programmers (want architecture details)
- VM implementers (want design patterns)
- Students (want clear explanations)
- Software archaeologists (want porting story)

**Key themes:**
- Concurrent constraint programming as a paradigm
- 1990s research VM design
- Software archaeology and porting
- Bug discovery as a teaching moment

### Part II: Architecture
**Intent:** Deep technical dive into AGENTS architecture, reconstructed from code analysis.

**Core topics:**
- System overview and component interaction
- Bytecode VM design
- Memory management (tagged pointers, GC)
- Instruction dispatch (threaded code, computed goto)
- Build system (1993 autoconf)

**Approach:**
- Reverse-engineer design principles from code
- Explain "why" not just "what"
- Show trade-offs and decisions
- Compare with modern VMs

### Part III: The Porting Story
**Intent:** Narrative of the actual porting work - what worked, what failed, what was discovered.

**Structure:**
- Challenge assessment
- Planning based on Alpha port
- Execution and compilation success
- **Bug discovery** (centerpiece - already well-documented)
- Lessons learned

**Unique value:**
- Real-world case study with complete documentation
- Bug discovery narrative (highly engaging)
- Honest account including failures
- Applicable lessons for any porting work

### Part IV: Technical Deep Dives
**Intent:** Focused technical chapters on specific subsystems.

**Topics:**
- Tagged pointers and type encoding
- Two-space copying GC
- Constraint solving (FD - Finite Domain)
- Register allocation across platforms
- **Signedness bug** (technical analysis of the discovery)

**Approach:**
- Code walkthroughs
- Design rationale
- Performance considerations
- Platform-specific variations

### Part V: Platform Specifics
**Intent:** Platform-by-platform implementation details.

**Platforms:**
- SPARC (original, 32-bit)
- Alpha (first 64-bit)
- x86-64 (modern, documented port)
- ARM64 (recent addition)
- General porting guide

**Value:**
- Concrete examples of platform adaptation
- Register mapping specifics
- Calling convention handling
- Cross-platform lessons

### Part VI: Appendices
**Intent:** Practical reference material.

**Contents:**
- Build instructions
- Testing guide
- References and bibliography
- Historical artifacts from porting work

---

## Part I: Introduction - Detailed Outline

### Chapter 1: What is AGENTS?

**Purpose:** Answer "what is this system?" for readers unfamiliar with constraint programming.

**Intent (2-3 paragraphs):**

AGENTS v0.9 is a concurrent constraint programming system developed at the Swedish Institute of Computer Science (SICS) in the early 1990s. It implements AKL (Andorra Kernel Language), a logic programming language that combines features from Prolog with concurrent constraint programming. The system is built as a bytecode virtual machine, similar in spirit to the WAM (Warren Abstract Machine) but adapted for concurrent execution and constraint propagation.

The system is interesting from multiple perspectives. Architecturally, it represents 1990s VM design philosophy: explicit register allocation, threaded code dispatch, manual memory management with copying garbage collection, and careful attention to cache performance. From a language design perspective, it embodies concurrent constraint programming - a paradigm where computation proceeds through progressive constraint refinement, with multiple agents running concurrently and communicating through shared constraint stores.

For modern systems programmers, AGENTS serves as a case study in several areas: bytecode VM implementation, garbage collector design, porting 32-bit code to 64-bit architectures, and maintaining legacy research software. The porting work documented in this book uncovered a 30-year-old bug, proving that even well-tested historical codebases can benefit from careful archaeological analysis.

**Subsections:**
1. **AKL and Concurrent Constraint Programming**
   - Logic programming background
   - Constraints vs. unification
   - Concurrent agents and suspension
   - Andorra principle

2. **AGENTS as a Virtual Machine**
   - Bytecode compilation model
   - Runtime system architecture
   - Relationship to WAM (Warren Abstract Machine)
   - Why a VM approach?

3. **Historical Context**
   - SICS and logic programming research
   - Early 1990s computing environment
   - Original target platforms (SPARC, MIPS)
   - Relationship to other systems (SICStus Prolog, AKL implementations)

4. **Why This Matters Today**
   - Software archaeology value
   - VM implementation lessons
   - Concurrent programming patterns
   - Porting and modernization challenges

### Chapter 2: The History

**Purpose:** Provide historical timeline and research context.

**Intent:**

AGENTS v0.9 was developed at SICS between 1990-1994 as part of research into concurrent logic programming and constraint systems. This was a period of active research in parallel and concurrent programming models, with logic programming seen as a natural fit for concurrent execution due to its declarative nature.

The system evolved through multiple versions, with v0.9 representing a mature implementation that supported the core AKL language features. It was designed to run on the workstations of the era - primarily SPARC and MIPS systems. An Alpha port was added later to support 64-bit systems, though this was done minimally and never fully tested without GMP (GNU Multiple Precision arithmetic library).

Understanding this historical context is crucial for appreciating the design decisions visible in the code. Many choices that seem unusual by modern standards (explicit register allocation, manual memory management, threaded code dispatch) made perfect sense in the early 1990s when cache hierarchies were simpler, memory was scarce, and compilers less sophisticated.

**Subsections:**
1. **Development Timeline (1990-1994)**
   - Initial design and prototypes
   - Evolution of AKL language
   - Major milestones
   - v0.9 release and features

2. **Research Goals**
   - Concurrent constraint programming
   - Andorra principle implementation
   - Performance vs. expressiveness
   - Comparison with other systems

3. **Original Platform Environment**
   - SPARC workstations
   - MIPS systems
   - Compiler toolchains (GCC 2.x era)
   - Available libraries and tools

4. **Evolution and Ports**
   - Alpha 64-bit port
   - Maintenance period
   - Eventual dormancy
   - 2025 modernization effort

### Chapter 3: About This Book

**Purpose:** Guide readers on how to use the book and what they'll learn.

**Intent:**

This book serves multiple audiences with different interests. Systems programmers interested in VM implementation will find detailed architecture chapters. Those maintaining legacy code will appreciate the porting case study. Students learning about interpreters and garbage collectors will benefit from concrete examples with real code. Software archaeologists will enjoy the detective work of understanding old systems.

The book is structured to support different reading paths. You can read straight through for a complete understanding, start with the porting story for an engaging narrative, dive into technical chapters for specific topics, or use it as a reference when working with similar systems.

All source code discussed in the book is available on GitHub, including the complete history of the porting work. You can build and run AGENTS yourself on modern systems (x86-64, ARM64) to experiment with the concepts discussed.

**Subsections:**
1. **Who This Book Is For**
   - Systems programmers
   - VM implementers
   - Students and educators
   - Software archaeologists
   - Legacy code maintainers

2. **How to Read This Book**
   - Sequential reading path
   - Topic-focused path
   - Practical/porting path
   - Reference use

3. **What You'll Learn**
   - VM architecture and implementation
   - Memory management and GC
   - Porting techniques
   - Bug investigation methods
   - Concurrent programming patterns

4. **Source Code and Experimentation**
   - GitHub repository
   - Building AGENTS
   - Running examples
   - Exploring the code

---

## Part II: Architecture - Detailed Outline

### Overview of Part II

**Intent:** This part reconstructs the AGENTS architecture from code analysis. Since we don't have original design documents, we must infer design principles, explain trade-offs, and document patterns discovered in the implementation.

**Approach:**
- Start with high-level system overview
- Dive into each major subsystem
- Explain "why" based on 1990s context
- Compare with modern approaches
- Use code examples to illustrate

**Key theme:** Understanding design through archaeology - reconstructing intent from implementation.

### Chapter 4: System Overview

**Purpose:** Provide 10,000-foot view of the entire system.

**Intent:**

AGENTS is structured as a classic bytecode interpreter with several major subsystems working together. At the heart is the bytecode emulator, which executes compiled AKL programs. Supporting this are the memory manager (with tagged pointers and copying GC), the constraint solver (for finite domain constraints), and the I/O system (for loading bytecode and interacting with the environment).

The system follows a compilation model where AKL source is compiled to bytecode (.pam files) by a compiler written in AKL itself (bootstrapped). The emulator then executes this bytecode, managing memory, handling concurrent agents, and solving constraints as the program runs.

Understanding the component interaction is key: the emulator calls the GC when memory is low, the constraint solver suspends agents when constraints aren't yet solvable, the I/O system loads new bytecode modules, and everything coordinates through a shared heap with tagged pointers encoding types.

**Subsections:**
1. **High-Level Architecture**
   - Major components and their roles
   - Data flow through the system
   - Compilation vs. runtime
   - Bootstrap process

2. **Component Interaction**
   - Emulator ↔ Memory Manager
   - Constraint Solver integration
   - I/O and module loading
   - Error handling

3. **Design Philosophy**
   - 1990s VM design patterns
   - Performance focus
   - Memory efficiency
   - Portability considerations

4. **File Organization**
   - Source file structure
   - Module dependencies
   - Build system layout
   - Generated files

### Chapter 5: The Emulator

**Purpose:** Deep dive into the bytecode interpreter core.

**Intent:**

The emulator (in emulator/*.c) is the heart of AGENTS. It's a bytecode interpreter using threaded code dispatch - a technique where each bytecode handler ends with a computed goto to the next instruction. This was a common optimization in the 1990s when indirect branches were expensive and compilers couldn't optimize switches well.

The emulator maintains a set of registers (some in actual hardware registers via GCC's register keyword) for frequently-accessed state: program counter, operand stack pointer, heap pointers, etc. On systems with enough registers (x86-64 has 6 callee-saved, ARM64 has 10), this can provide significant performance benefits.

Instructions are encoded compactly in the bytecode, with different formats for different instruction types. The dispatch loop is surprisingly simple - fetch instruction, jump to handler, execute, fetch next. But the individual instruction implementations are complex, handling unification, constraint propagation, agent spawning, and more.

**Subsections:**
1. **Bytecode Instruction Set**
   - Instruction categories (unify, construct, test, control)
   - Encoding formats
   - Operand representation
   - Special instructions

2. **Threaded Code Dispatch**
   - Computed goto technique
   - Why not switch?
   - Performance implications
   - Portability considerations

3. **VM Registers and State**
   - Hardware register allocation
   - Program counter
   - Operand stack
   - Heap pointers
   - Current agent state

4. **Instruction Implementation Examples**
   - Unification instructions
   - Constraint posting
   - Agent operations
   - Control flow

### Chapter 6: Memory Management

**Purpose:** Explain tagged pointers and garbage collection.

**Intent:**

AGENTS uses tagged pointers to encode type information in the low bits of addresses. This is possible because heap allocations are word-aligned, leaving the low 2-3 bits free for tags. The TADBITS configuration parameter controls how many bits are available, with 32-bit systems using 2 bits and 64-bit systems using 3 bits.

The garbage collector is a two-space copying collector - memory is divided into two semi-spaces, with allocation proceeding linearly in the active space. When that space fills, live objects are copied to the other space, compacting memory and eliminating fragmentation. This was a popular GC approach in the 1990s for its simplicity and good cache behavior.

Root scanning starts from the VM registers and stacks, tracing all reachable objects. The copying process updates pointers as it goes, maintaining a forwarding table. The entire GC is stop-the-world, which is acceptable for the programs AGENTS was designed to run.

**Subsections:**
1. **Tagged Pointers**
   - Tag bits and alignment
   - Type encoding scheme
   - TADBITS configuration
   - 32-bit vs 64-bit differences

2. **Heap Organization**
   - Two semi-space structure
   - Linear allocation
   - Heap size configuration
   - Memory layout

3. **Two-Space Copying GC**
   - Copy collector algorithm
   - Root scanning
   - Pointer updating
   - Forwarding pointers

4. **Type System**
   - Fundamental types (integer, atom, functor, reference)
   - Compound types (structures, lists)
   - Special values (unbound variables, small integers)
   - Type predicates and tests

### Chapter 7: Bytecode Dispatch

**Purpose:** Detailed look at the instruction dispatch mechanism.

**Intent:**

Bytecode dispatch is critical for interpreter performance. AGENTS uses computed goto (a GCC extension) to implement threaded code. Each instruction handler ends with a macro that fetches the next instruction and jumps to its handler. This eliminates the overhead of a central dispatch loop and improves branch prediction.

The dispatch code reveals careful attention to performance. Instructions are densely encoded to improve cache behavior. Common operations have specialized fast paths. The threaded code approach makes the interpreter essentially a big jump table, with minimal overhead between instructions.

This chapter will examine the dispatch mechanism in detail, explaining why these choices were made and how they compare to modern interpreter techniques (JIT compilation, superinstructions, etc.).

**Subsections:**
1. **Computed Goto and Threaded Code**
   - GCC labels-as-values extension
   - Dispatch macro
   - Handler structure
   - Branch prediction benefits

2. **Instruction Encoding**
   - Compact encoding for cache efficiency
   - Operand encoding
   - Immediate values
   - Addressing modes

3. **Performance Considerations**
   - Why computed goto vs. switch?
   - Inline vs. call trade-offs
   - Cache behavior
   - Branch prediction

4. **Modern Alternatives**
   - JIT compilation
   - Superinstructions
   - Template JIT
   - What would we do differently today?

### Chapter 8: The Build System

**Purpose:** Understand the 1993-era autoconf build system.

**Intent:**

AGENTS uses autoconf for portability, but it's autoconf from 1993 - much simpler than modern autoconf. The configure.in file is hand-written, the config.guess script uses hardcoded platform detection, and Makefiles are template-based.

Understanding the build system was crucial for the porting work. The BUILD-SYSTEM-ISSUE.md and BUILD-SYSTEM-FIX.md documents show how we had to integrate -fgnu89-inline into the autoconf flow to handle inline functions properly. This is a good example of how build system archaeology works.

**Subsections:**
1. **1993 Autoconf**
   - configure.in structure
   - Platform detection (config.guess)
   - Makefile generation
   - Differences from modern autoconf

2. **Compiler Detection and Flags**
   - GCC vs other compilers
   - CFLAGS propagation
   - -fgnu89-inline integration
   - Warning flags

3. **Platform-Specific Configuration**
   - TADBITS derivation
   - Register definitions
   - System dependencies
   - Library detection (GMP, math)

4. **Build Process Flow**
   - ./configure execution
   - Makefile generation
   - Recursive make
   - Binary creation

---

## Next Steps

This outline provides the structure for Parts I-II. Next steps:

1. **Complete outlines for Parts III-VI** (same iterative approach)
2. **Begin drafting Chapter 1** (start with actual prose)
3. **Code analysis for architecture reconstruction** (examine emulator/*.c in detail)
4. **Collect existing documentation** (porting notes, bug discovery narrative)

**Priority:** Start drafting Chapter 1 as a proof of concept, applying the iterative approach at the section and paragraph level.

---

**Status:** Outline complete for Parts I-II. Ready to continue with Parts III-VI or begin drafting Chapter 1.
