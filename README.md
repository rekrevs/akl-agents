# AGENTS (version 0.9)

## 1 INTRODUCTION

The **Agents Kernel Language (AKL)** is a concurrent constraint programming language developed at the Swedish Institute of Computer Science (SICS). In AKL, computation is performed by agents interacting through stores of constraints. This notion accommodates multiple programming paradigms; in appropriate contexts, AKL agents may be thought of as processes, objects, functions, relations, or constraints.

**AGENTS** is a system for programming in AKL. It provides:
- A complete implementation of AKL with equality constraints over rational trees.
- Various "finite domain" constraints over integers.
- Ports (a novelty of AKL).
- Assorted built-in agents and other support.

## 2 MANIFEST

This directory should contain at least the following:

- `Makefile.in` (source for `Makefile`)
- `README` (this file)
- `agents.1` (man page)
- `agents.in` (source for agents driver)
- `akl.el` (Emacs mode)
- `config.guess` (configuration utility)
- `config.sub` (configuration utility)
- `configure` (configuration script)
- `configure.in` (source for `configure`)
- `demos/` (example programs)
- `doc/` (documentation)
- `emulator/` (emulator, garbage collector, etc. in C)
- `environment/` (built-in agents, top level, etc. in AKL)
- `gmp/` (GMP library for bignums)
- `library/` (AGENTS library packages)
- `mkbuilddirs` (building utility)
- `mkinstalldirs` (installation utility)
- `oldcompiler/` (compiler in AKL)
- `tests/` (DejaGnu tests for AGENTS)
- `tools/` (AGENTS tool packages)
- `version.pam.in` (source for `version.pam`)

## 3 INSTALLATION

### Condensed Version
```bash
./configure
make
```

To build documentation:
```bash
make dvi    # Generates .dvi files in ./doc/
make ps     # Generates .ps files in ./doc/
```

Ensure at least **8 MB of disk space** for building.

### Full Installation Guide

1. **Configure**  
   `./configure` generates `Makefile` based on your system. Supported arguments:
   - `--host=HOST`: Build for host type (e.g., `rs6000-ibm-aix3.2`).
   - `--prefix=DIR`: Install AGENTS in `DIR` (default: `/usr/local`).
   - `--srcdir=DIR`: Source directory (default: current directory).
   - `--without-gmp`: Disable bignum support (not recommended).

   Example:
   ```bash
   ./configure --prefix=/usr/local
   ```

2. **Build**  
   ```bash
   make
   ```

3. **Install**  
   ```bash
   make install
   ```
   Requires ~6 MB disk space. Installed versions are standalone.

4. **Shadow Builds**  
   Use `mkbuilddirs` for out-of-tree builds:
   ```bash
   ./mkbuilddirs $(pwd) ../rs6000
   cd ../rs6000
   ./configure
   make
   ```

## 4 USING AGENTS

### Basic Commands
- Start the AGENTS top level:
  ```bash
  agents
  ```
- Compile a program (`program.akl` → `program.pam`):
  ```prolog
  ?- compilef(program).
  ```
- Load a compiled program:
  ```prolog
  ?- load(program).
  ```
- Compile and load in one step:
  ```prolog
  ?- compile(program).
  ```

### Debugger
- Start/stop the debugger:
  ```prolog
  ?- trace.     # Start
  ?- notrace.   # Stop
  ```
- Set spy points for debugging.

## 5 BUGS

The code is provided as is. No bug reports can be handled.

## 6 ACKNOWLEDGEMENTS

- Initial emulator by **Johan Bevemyr**.
- Prolog-to-AKL compiler adaptation by **Johan Bevemyr**.
- Contributions by **Sverker Janson**, **Johan Montelius**, **Kent Boortz**, **Per Brand**, **Bjorn Carlson**, **Ralph Clarke Haygood**, **Bjorn Danielsson**, **Dan Sahlin**, and **Thomas Sjoeland**.
- Graphics Manager uses code from **SICStus Prolog** (Claes Frisk, Jan Sundberg) and **InterViews** (Stanford University).
- **GMP** library by Torbjorn Granlund (GNU project).

## 7 CONTACT

Sverker Janson  
Email: `sverker.janson@gmail.com`
