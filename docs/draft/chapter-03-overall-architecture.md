# Chapter 3: Overall Architecture

## Introduction

This chapter presents the big picture: how AGENTS transforms AKL source code into running programs. We'll see how the compiler, bytecode format, and emulator work together, understand the directory structure, and explore how the system is built and deployed.

## Three-Layer Architecture

AGENTS follows a classic three-layer design found in many language implementations (Java, Python, Erlang, etc.):

```
┌──────────────────────────────────────────────────────────────┐
│                    Layer 1: Source Language                  │
│                  AKL Programs (.akl files)                   │
│  • Guards, constraints, ports                                │
│  • Declarative problem specification                         │
│  • User writes code at this level                           │
└──────────────┬───────────────────────────────────────────────┘
               │
               │ COMPILE (compiler/*.akl)
               │ • Parse AKL syntax
               │ • Generate bytecode
               │ • Create indexing structures
               │
               ▼
┌──────────────────────────────────────────────────────────────┐
│              Layer 2: Intermediate Representation            │
│                PAM Bytecode (.pam files)                     │
│  • Platform-independent binary format                        │
│  • ~115 instruction opcodes                                  │
│  • Resembles WAM with AKL extensions                        │
│  • Can be saved, loaded, distributed                        │
└──────────────┬───────────────────────────────────────────────┘
               │
               │ LOAD & EXECUTE (emulator/*.c)
               │ • Load .pam files dynamically
               │ • Link predicates
               │ • Execute instructions
               │
               ▼
┌──────────────────────────────────────────────────────────────┐
│                Layer 3: Execution Engine                     │
│               C Emulator (Native Code)                       │
│  • Virtual machine interpreter                               │
│  • Memory management & garbage collection                    │
│  • Built-in predicates                                       │
│  • I/O, arithmetic, constraint solving                      │
└──────────────────────────────────────────────────────────────┘
```

This separation provides several benefits:

**Portability**: PAM bytecode runs on any platform with an emulator. Recompiling the emulator for a new architecture enables AGENTS on that platform.

**Optimization Independence**: Compiler optimizations don't depend on the target architecture. A single compiler serves all platforms.

**Development Flexibility**: Language changes affect only the compiler (Layer 1→2) or emulator (Layer 2→3), not both.

**Deployment Options**: You can distribute source (.akl), bytecode (.pam), or both. Bytecode is faster to load.

## Directory Structure

The AGENTS source tree is organized logically:

```
akl-agents/
├── README                      System overview and installation guide
├── CHANGES.md                  Version history
├── Makefile, Makefile.in       Build system
├── configure, configure.in     Auto-configuration
├── agents, agents.in           Driver script
├── version.pam, version.pam.in Version information
│
├── compiler/                   AKL-to-PAM compiler (~30 modules)
│   ├── compiler.akl            Main compiler driver
│   ├── clause.akl              Clause processing
│   ├── head_alloc.akl          Head argument register allocation
│   ├── body_alloc.akl          Body argument register allocation
│   ├── opt.akl                 Optimizer
│   ├── builtin.akl             Built-in handling
│   └── ...                     (~25 more modules)
│
├── emulator/                   C-based execution engine
│   ├── main.c                  Entry point
│   ├── engine.c                Bytecode interpreter (2500+ lines)
│   ├── storage.c, gc.c         Memory management
│   ├── unify.c                 Unification engine
│   ├── builtin.c               Built-in predicates (3000+ lines)
│   ├── fd.c, fd_akl.c          Finite domain constraints
│   ├── load.c                  PAM file loading
│   ├── parser.yy, parser.tab.c Term parser (Flex/Bison)
│   ├── term.h                  Tagged pointer definitions
│   ├── sysdeps.h               Platform dependencies
│   ├── regdefs.h               Register allocation
│   └── ...                     (~50 .c files, ~63 .h files)
│
├── environment/                Runtime system in AKL
│   ├── boot.pam                Bootstrap code (pre-compiled)
│   ├── toplev.pam              Top-level read-eval-print loop
│   ├── builtin.akl             Built-in wrappers
│   ├── read.akl, write.akl     I/O
│   ├── fd_labeling.akl         FD search strategies
│   ├── prolog.akl              Prolog compatibility
│   └── ...                     (~30 .akl files)
│
├── library/                    User library packages
│   ├── lists.akl, lists.pam    List operations
│   ├── assoc.akl, assoc.pam    Association lists
│   ├── ordsets.akl             Ordered sets
│   ├── ugraphs.akl             Undirected graphs
│   ├── random.akl              Random numbers
│   └── ...                     (Several packages with .texi docs)
│
├── demos/                      Example programs
│   ├── ancestors.akl           Horn clause examples
│   ├── cube.akl                Instant Insanity puzzle
│   ├── fd_queens.akl           N-Queens with FD constraints
│   ├── cipher.akl              Cryptarithmetic
│   └── ...                     (~85+ .akl files with .pam)
│
├── tests/                      Test suite (DejaGnu)
│   ├── agents.tests/           Test cases
│   └── config/                 Test configuration
│
├── tools/                      SAGA graphical tools
│   └── ...
│
├── doc/                        Documentation (LaTeX, info)
│   ├── user/                   User manual
│   ├── library/                Library reference
│   ├── aklintro/               AKL introduction
│   └── internals.tex           Implementation notes (DRAFT)
│
├── docs/                       Modern documentation (Markdown)
│   ├── porting/                64-bit porting documentation
│   ├── attempts/               Porting attempt logs
│   ├── draft/                  This book
│   └── ...
│
└── gmp/                        GMP library (EXCLUDED)
    └── ...                     (Arbitrary precision arithmetic)
```

### Component Sizes

From a fresh checkout:

| Component | Files | Total Size | Purpose |
|-----------|-------|------------|---------|
| emulator/ | ~113 | 3.3 MB | Core execution engine in C |
| compiler/ | ~30 | 614 KB | AKL-to-PAM compiler in AKL |
| environment/ | ~34 | 546 KB + boot.pam (406 KB) | Runtime system |
| library/ | ~20 packages | 3.7 MB | Reusable libraries |
| demos/ | ~85 examples | 2.4 MB | Example programs |
| tests/ | Test suite | 59 KB | Validation tests |
| doc/ | LaTeX sources | 1.1 MB | User documentation |
| docs/ | Markdown | 500 KB+ | Modern documentation |

The emulator is the largest and most complex component, containing the virtual machine, garbage collector, constraint solvers, and built-ins.

## Compilation Pipeline

### From Source to Execution

Here's what happens when you compile and run an AKL program:

```
1. WRITE SOURCE
   └─> user_program.akl

2. COMPILE
   └─> agents -c "compilef(user_program)."
       │
       ├─ Load compiler (compiler/*.akl)
       ├─ Parse user_program.akl
       ├─ Generate PAM instructions
       ├─ Optimize (optional)
       └─> Write user_program.pam

3. LOAD
   └─> agents -c "load(user_program)."
       │
       ├─ Read user_program.pam
       ├─ Decode bytecode
       ├─ Create predicate structures
       ├─ Link to other predicates
       └─> Ready to execute

4. EXECUTE
   └─> agents -c "goal(...).   "
       │
       ├─ Look up predicate
       ├─ Create execution state
       ├─ Enter engine.c:tengine()
       ├─ Execute PAM instructions
       ├─ Perform unification, constraint solving, etc.
       └─> Produce results
```

### Compilation Modes

AGENTS supports three compilation workflows:

**1. Compile and Save** (`compilef/1`):
```
?- compilef(program).
```
- Reads `program.akl`
- Writes `program.pam` to disk
- PAM file can be loaded later or distributed

**2. Compile and Load** (`compile/1`):
```
?- compile(program).
```
- Reads `program.akl`
- Compiles to PAM bytecode in memory
- Loads immediately (no .pam file created)
- Faster for development

**3. Load Precompiled** (`load/1`):
```
?- load(program).
```
- Reads `program.pam` from disk
- Decodes and links
- Faster than compiling

### Bootstrapping

AGENTS faces a bootstrapping challenge: the compiler is written in AKL, but you need a compiler to compile it!

The solution:

1. **Pre-compiled bootstrap image** (`environment/boot.pam`):
   - Contains compiled compiler and essential runtime
   - Distributed with AGENTS
   - Loaded at emulator startup

2. **Emulator starts** and immediately loads `boot.pam`

3. **Top-level loop** (`environment/toplev.pam`) starts

4. **User can now compile** new AKL programs using the loaded compiler

When rebuilding from scratch (`make realclean; make all`):
1. Old `boot.pam` is used to compile the compiler
2. New `boot.pam` is generated from the freshly compiled compiler
3. Process repeats until stable (usually 2-3 iterations)

This is similar to how C compilers are often bootstrapped: an old version of the compiler compiles the new version.

## The Emulator Architecture

### Emulator Components

The C emulator is divided into subsystems:

```
┌────────────────────────────────────────────────────────────┐
│                    main.c (Entry Point)                    │
│  • Parse command-line arguments                            │
│  • Initialize system                                       │
│  • Load boot.pam                                           │
│  • Enter top-level loop                                    │
└────────────┬───────────────────────────────────────────────┘
             │
             ▼
┌────────────────────────────────────────────────────────────┐
│               initial.c (Initialization)                   │
│  • Allocate memory spaces                                  │
│  • Initialize atom table                                   │
│  • Create built-in predicates                              │
│  • Set up signal handlers                                  │
└────────────┬───────────────────────────────────────────────┘
             │
             ▼
┌────────────────────────────────────────────────────────────┐
│            load.c (Bytecode Loader)                        │
│  • Read .pam files                                         │
│  • Decode instructions                                     │
│  • Create predicate structures                             │
│  • Resolve references                                      │
└────────────┬───────────────────────────────────────────────┘
             │
             ▼
┌────────────────────────────────────────────────────────────┐
│               engine.c (Execution)                         │
│  • Main instruction interpreter                            │
│  • And-box and choice-box management                       │
│  • Guard execution and suspension                          │
│  • ~115 instruction handlers                               │
└─────┬──────────────────────────────────────────────────────┘
      │
      ├─────> unify.c (Unification)
      │       • Rational tree unification
      │       • Occurs check
      │       • Dereferencing
      │
      ├─────> storage.c, gc.c (Memory)
      │       • Heap allocation
      │       • Garbage collection
      │       • Alignment handling
      │
      ├─────> fd.c, fd_akl.c (Constraints)
      │       • Finite domain constraints
      │       • Propagation algorithms
      │       • Suspension on constraints
      │
      ├─────> builtin.c (Built-ins)
      │       • Arithmetic
      │       • I/O
      │       • Type checking
      │       • Reflection
      │       • Port operations
      │
      ├─────> port.c, send.c (Ports)
      │       • Port creation
      │       • Asynchronous sending
      │       • Stream management
      │
      └─────> trace.c, debug.c (Debugging)
              • Execution tracing
              • Spy points
              • Breakpoints
```

Each subsystem is relatively independent, communicating through well-defined interfaces.

### Execution State

The core data structure is the **execution state** (`struct exstate` in `exstate.h`):

```c
typedef struct exstate {
  struct exstate   *father;      // Parent execution state
  struct exstate   *next;        // Sibling states
  struct exstate   *child;       // Child states

  code             *pc;          // Program counter
  uword            arity;        // Current arity
  predicate        *def;         // Current predicate

  choicebox        *insert;      // Choice insertion point
  andbox           *andb;        // Current and-box
  choicebox        *root;        // Root choice-box

  trailhead        trail;        // Backtracking trail
  taskhead         task;         // Task queue
  recallhead       recall;       // Recall stack
  wakehead         wake;         // Wake queue

  Term             areg[MAX_AREGS]; // Argument registers (256 or 8192)

  // ... more fields
} exstate;
```

This structure contains everything needed to execute an agent:
- **Program counter** (pc): current instruction
- **Registers** (areg): argument passing
- **And-box** (andb): concurrent context
- **Choice-box** (insert): alternative branches
- **Queues**: trail, tasks, recalls, wakes

Multiple execution states can exist simultaneously, representing concurrent agents.

## Build System

### Autoconf Configuration

AGENTS uses autoconf for platform detection and configuration:

```bash
$ ./configure [options]
```

Key configure options:

**`--prefix=DIR`**: Installation directory (default: `/usr/local`)
```bash
$ ./configure --prefix=/opt/agents
```

**`--without-gmp`**: Build without GMP (recommended for 64-bit)
```bash
$ ./configure --without-gmp
```

**`--without-fd`**: Build without finite domain constraints
```bash
$ ./configure --without-fd
```

**`--host=HOST`**: Target architecture (auto-detected)
```bash
$ ./configure --host=x86_64-unknown-linux-gnu
```

### Configuration Process

1. **Platform Detection** (`config.guess`):
   - Detects CPU architecture
   - Detects OS and version
   - Returns canonical triplet: `cpu-vendor-os`

2. **Platform Normalization** (`config.sub`):
   - Converts short names to canonical form
   - Example: `x86_64` → `x86_64-unknown-linux-gnu`

3. **Configure Script** (`configure`):
   - Checks for compiler (gcc or clang)
   - Checks for required tools (bison, flex)
   - Sets platform-specific flags
   - Generates Makefile from Makefile.in
   - Generates config.h with platform macros

4. **Platform-Specific Settings** (in configure.in):
```bash
case "$host" in
  x86_64-*-linux*)
    TADBITS=64
    WORDALIGNMENT=8
    ;;
  aarch64-*-linux*|arm64-*-darwin*)
    TADBITS=64
    WORDALIGNMENT=8
    ;;
  # ... more platforms
esac
```

### Makefile Targets

The generated Makefile provides many targets:

**Build targets:**
```bash
make all           # Build everything (default)
make agents        # Build the emulator only
make compiler      # Build the compiler
make environment   # Build the runtime environment
make library       # Build library packages
```

**Installation:**
```bash
make install       # Install to $(prefix)
make uninstall     # Remove installation
```

**Cleaning:**
```bash
make clean         # Remove build artifacts, keep sources
make distclean     # Also remove configure-generated files
make realclean     # Remove everything rebuildable (DANGEROUS)
```

**Testing:**
```bash
make check         # Run test suite (requires DejaGnu)
```

**Documentation:**
```bash
make info          # Build info files (requires makeinfo)
make dvi           # Build DVI files (requires LaTeX)
make ps            # Build PostScript files (requires dvips)
```

### Build Process

A typical build from scratch:

```bash
$ ./configure --without-gmp
checking for gcc... gcc
checking whether the C compiler works... yes
checking for bison... bison
checking for flex... flex
...
config.status: creating Makefile
config.status: creating emulator/config.h

$ make all
cd emulator && gmake all
gcc -O2 -DHAVE_CONFIG_H -c main.c
gcc -O2 -DHAVE_CONFIG_H -c initial.c
gcc -O2 -DHAVE_CONFIG_H -c engine.c
...
gcc -O2 -o agents main.o engine.o ... -lm
...
Compiling compiler...
Compiling environment...
Done.
```

This builds:
1. **Emulator binary** (`agents`)
2. **Compiler** (compiled from compiler/*.akl using boot.pam)
3. **Environment** (runtime system from environment/*.akl)
4. **Libraries** (from library/*.akl to library/*.pam)

### Installation Layout

After `make install`, files are organized:

```
$(prefix)/
├── bin/
│   └── agents                          Executable
│
├── lib/agents0.9/
│   ├── compiler/
│   │   └── *.akl, *.pam               Compiler modules
│   ├── environment/
│   │   └── *.akl, *.pam               Runtime system
│   ├── library/
│   │   └── *.akl, *.pam               Libraries
│   ├── demos/
│   │   └── *.akl, *.pam               Examples
│   └── emulator/
│       └── *.h                         Headers (for FFI)
│
├── man/man1/
│   └── agents.1                        Man page
│
└── info/
    └── agents-*.info                   Documentation
```

The `agents` script (from `agents.in`) sets up paths to find these files.

## The `agents` Driver Script

The `agents` executable is actually a shell script wrapper around the emulator binary:

```bash
#!/bin/sh
# agents driver script

VERSION=0.9
AGENTSDIR=/usr/local/lib/agents0.9

# Set up paths
EMULATOR=$AGENTSDIR/emulator/agents.bin
BOOTFILE=$AGENTSDIR/environment/boot.pam
LIBRARY=$AGENTSDIR/library

# Pass arguments to emulator
exec $EMULATOR -boot $BOOTFILE -lib $LIBRARY "$@"
```

This indirection allows:
- **Path configuration** at install time
- **Environment variable setup**
- **Argument preprocessing**
- **Multiple versions** coexisting

## Runtime System

### The Top-Level Loop

When you run `agents` with no arguments, you enter an interactive read-eval-print loop (REPL):

```
% agents
AGENTS 0.9 - Andorra Kernel Language
Copyright (c) 1993-1994 SICS

?-
```

This is implemented in `environment/toplev.pam` (compiled from AKL source).

The loop:
1. **Read** a goal from the user (using `read/1`)
2. **Execute** the goal (call it as a predicate)
3. **Print** results (bindings of variables)
4. **Loop** back to read next goal

### Built-in Predicates at Startup

At initialization, the emulator registers ~100 built-in predicates:

- **Arithmetic**: `is/2`, `=:=/2`, `</2`, etc.
- **Unification**: `=/2`, `\=/2`, `unify/2`
- **Type checking**: `var/1`, `atom/1`, `integer/1`, etc.
- **I/O**: `write/1`, `read/1`, `nl/0`, `get_char/1`, etc.
- **Control**: `call/1`, `apply/1`, `!/0` (cut)
- **Meta**: `functor/3`, `arg/3`, `=../2` (univ)
- **Ports**: `port/1`, `send/2`
- **FD**: `fd/1`, `fd_labeling/1`, etc.
- **Reflection**: `current_predicate/1`, `predicate_property/2`
- **System**: `halt/0`, `statistics/2`, `garbage_collect/0`

These are implemented in C (`builtin.c`, `port.c`, `fd_akl.c`, etc.) and registered during `initial.c:init_builtins()`.

### Loading User Programs

To load your program:

```
?- load(myprogram).
```

This:
1. Looks for `myprogram.pam` in the current directory
2. Calls `load.c:load_file("myprogram.pam")`
3. Decodes bytecode into predicate structures
4. Links predicates (resolves calls)
5. Makes predicates available for calling

To compile and load:

```
?- compile(myprogram).
```

This compiles `myprogram.akl` in memory and loads it immediately.

## Deployment Modes

AGENTS supports several deployment modes:

### 1. Interactive Development

Run the REPL, load and test code interactively:

```bash
$ agents
?- compile(mycode).
?- test_predicate(X).
X = result1 ? ;
X = result2 ? ;
no
?-
```

### 2. Batch Execution

Execute a goal and exit:

```bash
$ agents -c "compile(mycode), run_tests, halt."
```

### 3. Script Execution

Create an executable AKL script with shebang:

```bash
#!/usr/bin/env agents -s
%% myprogram.akl

main :-
    write('Hello, World!'), nl,
    halt.

:- main.
```

Make executable and run:

```bash
$ chmod +x myprogram.akl
$ ./myprogram.akl
Hello, World!
```

### 4. Compiled Bytecode Distribution

Distribute `.pam` files for faster loading:

```bash
# Developer side
$ agents -c "compilef(bigprogram)."
$ tar czf bigprogram.tar.gz bigprogram.pam

# User side
$ tar xzf bigprogram.tar.gz
$ agents -c "load(bigprogram), run."
```

## Relationship Between Components

### Dependency Graph

```
┌─────────────┐
│   User      │
│ Programs    │
└──────┬──────┘
       │ uses
       ▼
┌─────────────┐          ┌──────────────┐
│   Library   │          │   Compiler   │
│  Packages   │          │   (AKL)      │
└──────┬──────┘          └──────┬───────┘
       │                        │
       │ compiled by            │ compiled by
       │                        │
       └───────────┬────────────┘
                   │
                   ▼
            ┌────────────┐
            │  boot.pam  │
            │ (bootstrap)│
            └──────┬─────┘
                   │
                   │ loaded by
                   │
                   ▼
            ┌─────────────┐
            │  Emulator   │
            │    (C)      │
            └─────────────┘
                   │
                   │ uses
                   ▼
            ┌─────────────┐
            │ Built-ins   │
            │ (C code)    │
            └─────────────┘
```

**Key dependencies:**
- User programs depend on library packages
- Library packages depend on the compiler
- Compiler is compiled by boot.pam
- boot.pam is loaded by the emulator
- Emulator provides built-in primitives

### Circular Dependencies

Note the circularity:
1. The compiler is written in AKL
2. To compile AKL, you need the compiler
3. The compiler must be pre-compiled to boot.pam
4. boot.pam is used to compile the compiler

This is resolved by **bootstrapping**: an old boot.pam compiles the new compiler, producing a new boot.pam.

## Platform Adaptation

### Platform-Specific Code

Platform dependencies are isolated to a few files:

**`emulator/sysdeps.h`**: Platform detection and word size
```c
#if defined(__x86_64__) || defined(__aarch64__)
#  define TADBITS 64
#  define PTR_ORG ((uword)0)
#  define WORDALIGNMENT 8
#endif
```

**`emulator/regdefs.h`**: Hard register allocation
```c
#ifdef __x86_64__
#  define HARDREGS
#  define REGISTER1 "r12"
#  define REGISTER2 "r13"
#  // ... etc.
#endif
```

**`configure.in`**: Build-time detection
```bash
case "$host" in
  x86_64-*)
    ARCH_FLAGS="-m64"
    ;;
  aarch64-*|arm64-*)
    ARCH_FLAGS="-march=armv8-a"
    ;;
esac
```

Most C code is portable and platform-independent.

### Adding a New Platform

To port AGENTS to a new architecture:

1. Add detection to `configure.in`
2. Add platform section to `sysdeps.h` (set TADBITS, PTR_ORG, etc.)
3. Optionally add register definitions to `regdefs.h`
4. Test with existing benchmarks
5. Debug any platform-specific issues

See docs/64-BIT-PORTING-GUIDE.md for complete details on the recent x86-64 and ARM64 ports.

## Summary

AGENTS' architecture is clean and modular:

- **Three layers**: Source (AKL) → Bytecode (PAM) → Execution (C)
- **Separation of concerns**: Compiler, runtime, built-ins, libraries
- **Platform independence**: Bytecode is portable, emulator is adaptable
- **Bootstrapping**: Self-hosting compiler via boot.pam
- **Flexible deployment**: Interactive, batch, script, or bytecode distribution

This design has served AGENTS well for decades and enabled the recent transition to 64-bit platforms with minimal changes.

The next chapters dive into each component in detail, starting with the PAM bytecode format.

---

**Next Chapter**: [Chapter 4: The Prototype Abstract Machine (PAM)](chapter-04-pam.md)
