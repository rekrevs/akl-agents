# AKL-Agents

> AGENTS system for the Agents Kernel Language (AKL) - concurrent constraint programming from SICS.

## Overview

This is a legacy codebase from the Swedish Institute of Computer Science (SICS) implementing AKL, a concurrent constraint programming language where computation is performed by agents interacting through stores of constraints.

## Quick Start

```bash
# Build the system
make

# Run the AKL interpreter
bin/akl
```

## Tech Stack

- **Language**: C, AKL (Prolog-like syntax)
- **Build**: Makefile
- **Platforms**: x86-64, ARM64 (Apple Silicon)

## Project Structure

```
akl-agents/
├── Makefile              # Main build file
├── README                 # Original SICS documentation
├── src/                   # C source code
├── lib/                   # AKL library code
├── bin/                   # Built executables
├── docs/                  # Documentation
│   └── 64-BIT-PORTING-GUIDE.md  # Modern platform port docs
└── examples/              # AKL example programs
```

## Key Concepts

- **Agents**: Concurrent entities that interact through constraints
- **Constraints**: Logical assertions about variables
- **Ports**: Communication channels between agents
- **Finite Domains**: Integer constraint solving

## Commands

```bash
make              # Build everything
make clean        # Clean build artifacts
make test         # Run test suite
```

## Verification

```bash
# Check build succeeds
make clean && make

# Run basic tests
bin/akl -l lib/qa.akl
```

## Task Management

```bash
/wotan              # See active tasks
/wotan add "..."    # Create task
/wotan start T-NNNN # Start working
```

## Related Projects

- `pyakl` - Python implementation of AKL
