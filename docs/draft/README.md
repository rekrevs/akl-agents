# Inside AGENTS: A Concurrent Constraint Programming System

## First Draft - November 2025

**Author:** Reconstructed from source code and documentation
**Subject:** AGENTS v0.9 implementation internals
**Status:** First comprehensive draft

---

## About This Book

This book documents the internal design and implementation of **AGENTS**, a system for concurrent constraint programming that implements the **Andorra Kernel Language (AKL)**. Developed at the Swedish Institute of Computer Science (SICS) in the early 1990s, AGENTS represents a sophisticated attempt to unify logic programming, concurrent programming, and constraint solving in a single coherent system.

The book is written for:
- **Implementers** who want to understand abstract machine design
- **Researchers** studying concurrent constraint programming systems
- **Students** learning about virtual machine implementation
- **Porters** adapting AGENTS to new platforms
- **Language designers** exploring the design space

## What Makes AGENTS Interesting?

### 1. Unified Programming Model

AGENTS combines multiple paradigms:
- **Logic programming** (Prolog-style Horn clauses)
- **Concurrent programming** (and-parallelism)
- **Constraint solving** (finite domain constraints)
- **Committed choice** (GHC-style guards)
- **Message passing** (port-based communication)

This unification is not superficial—these features genuinely compose and interact.

### 2. Sophisticated Abstract Machine

The Prototype Abstract Machine (PAM) extends the Warren Abstract Machine with:
- **And-boxes** for concurrent agent execution
- **Choice-boxes** for or-parallelism
- **Guard suspension** for synchronization
- **Constraint propagation** with waking
- **Port primitives** for asynchronous communication

### 3. Efficient Implementation

Despite its expressive power, AGENTS achieves excellent performance through:
- **Threaded code dispatch** (computed goto)
- **Register allocation** (mapping emulator variables to CPU registers)
- **Tagged pointers** (compact, self-describing data)
- **Copying GC** (simple and effective)
- **Inline instruction handlers** (no call overhead)

### 4. Platform Portability

AGENTS successfully ported from 32-bit SPARC to modern 64-bit platforms (x86-64, ARM64), demonstrating robust architectural abstraction.

## Current Draft Status

### Completed Chapters

✅ **Chapter 1: Introduction**
Complete overview of AGENTS, historical context, and the AKL paradigm. Explains what makes this system unique and provides a reading guide.

✅ **Chapter 2: The AKL Language**
Comprehensive introduction to AKL syntax and semantics. Covers guards, ports, constraints, concurrency, and provides practical examples.

✅ **Chapter 3: Overall Architecture**
The big picture: three-layer architecture, directory structure, build system, and how components fit together.

✅ **Chapter 6: Data Representation**
Deep dive into the tagged pointer scheme. Explains how atoms, integers, lists, structures, and variables are encoded in memory.

✅ **Chapter 9: The Execution Engine**
The heart of the system. Covers the bytecode interpreter in engine.c, instruction dispatch, and-boxes, choice-boxes, and optimization techniques.

✅ **OUTLINE.md**
Complete outline for all 25 chapters plus appendices, with detailed section descriptions.

### Remaining Work

The outline provides a comprehensive roadmap. Key chapters to complete:

**Part II: System Architecture (Chapters 4-5)**
- Chapter 4: PAM Bytecode format and instruction set
- Chapter 5: Execution model (and-boxes, choice-boxes, agents)

**Part III: Core Implementation (Chapters 7-8, 10)**
- Chapter 7: Memory management (heap, allocation, alignment)
- Chapter 8: Garbage collection (two-space copying algorithm)
- Chapter 10: Instruction dispatch (threaded code implementation)

**Part IV: Advanced Features (Chapters 11-15)**
- Chapter 11: Unification engine
- Chapter 12: Choice points and backtracking
- Chapter 13: And-boxes and concurrency
- Chapter 14: Constraint variables and suspension
- Chapter 15: Port communication

**Part V: Support Systems (Chapters 16-18)**
- Chapter 16: Built-in predicates
- Chapter 17: Compiler
- Chapter 18: Debugging and tracing

**Part VI: Platform Support (Chapters 19-22)**
- Chapter 19: Platform dependencies
- Chapter 20: Register allocation strategies
- Chapter 21: Build system details
- Chapter 22: 64-bit porting (recently completed)

**Part VII: Testing and Evolution (Chapters 23-25)**
- Chapter 23: Test suite and validation
- Chapter 24: Performance characteristics and benchmarks
- Chapter 25: Design rationale and tradeoffs

**Part VIII: Appendices**
- Complete instruction reference
- Data structure reference
- Configuration options
- File organization
- Glossary
- Bibliography

## Key Insights from Completed Chapters

### From Chapter 1: Introduction

AGENTS embodies the **Andorra Principle**: deterministic computation should proceed eagerly while nondeterministic choices should be delayed. The system successfully synthesizes three historically separate programming paradigms (logic, concurrent, constraint) into a coherent whole.

The three-layer architecture (AKL source → PAM bytecode → C emulator) provides excellent separation of concerns and portability.

### From Chapter 2: AKL Language

The **guard commitment operator** (`?`) is the key innovation that enables both search (backtracking within guards) and commitment (no backtracking across guard success). This combination is unique and powerful.

**Ports** provide clean asynchronous message passing without the complexity of full unification in communication. This distinguishes AKL from earlier concurrent logic languages.

### From Chapter 3: Architecture

The **bootstrapping** approach (pre-compiled boot.pam containing the compiler) elegantly solves the circular dependency: the compiler is written in AKL, but you need a compiler to compile AKL.

Platform dependencies are well-isolated to a few files (sysdeps.h, regdefs.h, configure.in), making porting straightforward.

### From Chapter 6: Data Representation

The **tagged pointer scheme** is beautifully designed:
- All data fits in one machine word
- Type information in low bits (alignment guarantees they're zero)
- Small integers directly encoded (58 bits on 64-bit!)
- Self-describing (every value carries its type)
- Fast type checking (bitwise operations)

The transition to 64-bit makes this scheme even more attractive by virtually eliminating bignum needs.

### From Chapter 9: Execution Engine

The engine is a masterpiece of low-level optimization:
- **Threaded code dispatch** avoids switch overhead
- **Register allocation** keeps hot variables in CPU registers
- **Inline handlers** eliminate function call overhead
- **And-box tree** represents concurrent execution
- **Choice-box tree** represents nondeterministic alternatives

The dual dispatch mechanism (threaded code + switch fallback) shows careful attention to portability vs. performance.

## How to Read This Book

### Quick Overview (2-3 hours)

Read:
1. Chapter 1 (Introduction)
2. Chapter 3 (Architecture) - skim
3. Chapter 6 (Data Representation) - focus on tagged pointer scheme
4. Chapter 9 (Execution Engine) - focus on main loop and dispatch

This gives you the essence of the system.

### Deep Dive (Multiple Days)

Read all chapters in order. Each chapter builds on previous ones. Work through the code examples. Trace execution of sample programs.

### Problem-Focused

Use the outline to find chapters relevant to your question:
- Porting to new platform? → Chapters 19-22
- Understanding unification? → Chapter 11
- Optimizing performance? → Chapters 9, 10, 24
- Adding built-ins? → Chapter 16

### For Implementers

Focus on Part III (Core Implementation) and Part VI (Platform Support). These chapters contain the gritty details needed to understand and modify the emulator.

### For Language Designers

Focus on Parts I-II (Foundations and Architecture) and Part VII (Testing and Evolution). These explain the high-level design choices and their rationale.

## Sources

This book was reconstructed from:

1. **Source code** (~50 C files, ~15,000 lines in emulator/)
2. **Existing documentation** (docs/porting/*.md, especially 01-ARCHITECTURE-OVERVIEW.md through 13-ITERATIVE-STRATEGY.md)
3. **README and build system**
4. **Demo programs** (demos/*.akl)
5. **Thesis fragments** (doc/internals.tex)
6. **Academic papers** (referenced in bibliography)

The book represents a **logical reconstruction**—inferring design principles and architecture from the implementation.

## Technical Specifications

**System:** AGENTS v0.9
**Language:** AKL (Andorra Kernel Language)
**Implementation:** ~50 C files, ~15,000 lines
**Platforms:** x86-64, ARM64 (64-bit); SPARC, MIPS, Alpha (legacy)
**Compiler:** gcc or clang
**License:** (as per original distribution)

**Key Features:**
- Register-based abstract machine
- ~115 bytecode instructions
- Threaded code dispatch (computed goto)
- Tagged pointer data representation (58-bit small integers on 64-bit)
- Two-space copying garbage collection
- Finite domain constraint solver
- Port-based asynchronous communication
- Four-port debugger with spy points

## Acknowledgments

AGENTS was developed at SICS by:
- Sverker Janson (principal author, emulator)
- Johan Bevemyr (original emulator, compiler adaptation)
- Johan Montelius (language design, runtime)
- Seif Haridi (language design, theory)
- Peter Olin (original Prolog compiler)
- Per Brand, Kent Boortz, Bjorn Carlson, Ralph Haygood, Bjorn Danielsson, Dan Sahlin, Thomas Sjoeland

The recent 64-bit porting work (2025) leveraged the existing Alpha port as a blueprint.

## Future Work

### Completing the Book

Priority chapters to write next:
1. Chapter 4 (PAM bytecode)
2. Chapter 11 (Unification)
3. Chapter 7 (Memory management)
4. Chapter 22 (64-bit porting - document the recent work)

### Additional Topics

Potential additions:
- Performance case studies with real applications
- Comparison with other abstract machines (WAM, BEAM, JVM)
- Detailed constraint propagation algorithms
- Parallel implementation strategies
- Modern extensions and optimizations

### Validation

Have original AGENTS developers review for accuracy. Correct any misunderstandings in the reconstruction.

## Contributing

This is a living document. Contributions welcome:
- Corrections to technical details
- Additional examples and diagrams
- Completed chapters from the outline
- Performance measurements
- Historical context and anecdotes

## Contact

For questions or feedback about this documentation project, see the repository.

---

## File Listing

### Completed Draft Chapters

```
docs/draft/
├── README.md                           (this file)
├── OUTLINE.md                          (complete book structure)
├── chapter-01-introduction.md          (20+ pages)
├── chapter-02-akl-language.md          (25+ pages)
├── chapter-03-overall-architecture.md  (20+ pages)
├── chapter-06-data-representation.md   (18+ pages)
└── chapter-09-execution-engine.md      (20+ pages)
```

**Total:** ~100+ pages of comprehensive documentation in first draft.

### Supporting Materials

```
docs/porting/
├── 01-ARCHITECTURE-OVERVIEW.md         Source material
├── 02-EMULATOR-ANALYSIS.md             Source material
├── 03-PLATFORM-DEPENDENCIES.md         Source material
├── 04-MEMORY-MANAGEMENT.md             Source material
├── 05-BYTECODE-DISPATCH.md             Source material
└── ... (13 documents total)
```

---

**Document Version:** 1.0
**Date:** November 6, 2025
**Branch:** claude/draft-inside-agents-book-011CUsNSbHYNJMH8ApVc7GDt

**Status:** First draft in progress. Core chapters complete. Foundation established for remaining work.
