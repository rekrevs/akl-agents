# AKL (Agents Kernel Language) Implementation

This repository contains the implementation of the Agents Kernel Language (AKL), a concurrent constraint programming language developed at the Swedish Institute of Computer Science (SICS) in the 1990s.

## Project Overview

AKL is a concurrent constraint programming language that combines features from logic programming and concurrent programming paradigms. This implementation was originally developed for Sun workstations in the 1990s and is now being modernized to work on contemporary systems.

## Repository Structure

- **SPECIFICATIONS.md**: Details of the AKL implementation's core mechanisms
- **BUILD.md**: Documentation of the original build system
- **REFACTORING.md**: Plan for modernizing the codebase
- **STRATEGY.md**: Strategic approach for the modernization project

## Key Components

- **Emulator**: The virtual machine that executes AKL programs
- **Compiler**: Translates AKL source code to the internal representation
- **Runtime System**: Memory management and execution environment
- **Standard Library**: Built-in predicates and functions

## Modernization Project

This repository is part of an ongoing effort to modernize the AKL implementation for compatibility with contemporary systems. The original implementation:

- Uses C macros for tagged pointers with pointer arithmetic
- Includes a custom garbage collection mechanism
- Was designed for 32-bit architectures
- Was last runnable on Sun workstations in the 1990s

The modernization goals include:

1. Making the code buildable and runnable on modern 64-bit Unix-like systems
2. Preserving the original semantics and behavior
3. Improving maintainability and readability
4. Updating the build system to use modern tools

## Getting Started

See the STRATEGY.md file for the current development approach and BUILD.md for information about the original build system.

## License

[Original license information to be added]

## Acknowledgments

The original AKL implementation was developed at the Swedish Institute of Computer Science (SICS).
