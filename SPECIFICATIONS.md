# AKL Language Implementation Specifications

This document provides a reverse-engineered specification of the AKL (Agents Kernel Language) implementation, focusing on the emulator's core mechanisms, particularly the tagged pointer system and memory management.

## Overview

The Agents Kernel Language (AKL) is a concurrent constraint programming language developed at the Swedish Institute of Computer Science (SICS) in the 1990s. In AKL, computation is performed by agents interacting through stores of constraints. This paradigm accommodates multiple programming models; AKL agents can be conceptualized as processes, objects, functions, relations, or constraints depending on context.

This implementation was designed for Sun workstations in the 1990s and relies on specific memory architecture assumptions that were common at that time.

## Core Components

The implementation consists of several key components:

1. **Emulator**: The core execution engine (in `emulator/`)
2. **Compiler**: Translates AKL code to an intermediate representation
3. **Environment**: Built-in agents and top-level functionality
4. **Library**: Standard AKL libraries and utilities

## Tagged Pointer Scheme

The implementation uses a sophisticated tagged pointer scheme to efficiently represent different data types while minimizing memory usage. This is implemented primarily through C macros and bit manipulation.

### Tag Layout (32-bit Architecture)

The tagged pointer scheme uses both primary and secondary tags in low bits, with a high bit reserved for garbage collection marking:

```
Reference:                     0:::::::::::::::::::::::::::::00
Unbound unconstrained variable: 0:::::::::::::::::::::::::::::01
Unbound constrained variable:   0:::::::::::::::::::::::::::::11
List cell:                      0:::::::::::::::::::::::::::0010
Structure:                      0:::::::::::::::::::::::::::1010
Generic, float, or bignum:      0:::::::::::::::::::::::::::1110
Small integer:                  0::::::::::::::::::::::::::00110
Atom:                           0::::::::::::::::::::::::::10110
```

The implementation uses the following tag values:
- Primary tags (2 bits): REF(0), UVA(1), GVA(3), IMM(6), LST(2), STR(0xa), GEN(0xe)
- Secondary tags (4 bits): Used for more specific type information
- Mark bit: The highest bit (bit 31 in 32-bit architecture) is used as a GC mark bit

### Key Tag Macros

The system defines numerous macros for tag manipulation:
- `TagPtr(X, T)`: Tags a pointer X with tag T
- `TagOth(X, T)`: Tags a shifted pointer X with tag T
- `TagAtm(A)`: Tags an atom
- `TagNum(I)`: Tags a number
- `TagRef(X)`: Tags a reference
- `TagUva(X)`: Tags an unbound variable
- `TagGva(X)`: Tags a constrained variable
- `TagBig(X)`: Tags a bignum
- `TagFlt(X)`: Tags a float
- `TagLst(X)`: Tags a list
- `TagStr(X)`: Tags a structure
- `TagGen(X)`: Tags a generic object

### System-Dependent Considerations

The implementation includes system-dependent adjustments for different architectures:
- For HP-PA: `PTR_ORG` is set to `0x40000000`
- For AIX: `PTR_ORG` is set to `0x20000000`
- For Alpha: `TADBITS` is set to 64 (vs. the default 32)

These adjustments accommodate differences in memory layout and pointer alignment across platforms.

## Memory Management

### Heap Allocation

Memory allocation is primarily managed through a set of macros that allocate space on the heap:
- `NEW(ret, type)`: Allocates space for a new object of type `type`
- `NEWX(ret, type, size)`: Allocates an array of objects
- `NewReference(ret)`: Allocates a reference
- `NewList(ret)`: Allocates a list cell
- `NewStructure(ret, arity)`: Allocates a structure with given arity
- `NewFloat(ret)`: Allocates a float (with proper alignment)
- `NewGeneric(PTR, TYPE)`: Allocates a generic object

These macros handle heap overflow by calling `reinit_heap()` when necessary.

### Garbage Collection

The garbage collection system is a mark-and-sweep collector with forwarding pointers. Key aspects include:

1. **Mark Phase**: Uses the high bit of each tagged pointer to mark live objects
2. **Forwarding Pointers**: During collection, objects are moved and forwarding pointers are installed
3. **Root Set**: Includes registers, execution state, and other global references
4. **Specialized Handling**: Different object types have specialized GC routines

The GC implementation is in `gc.c` and includes functions like:
- `gc()`: Main garbage collection entry point
- `gc_term()`: Handles collection of term objects
- `gc_variable()`: Handles collection of variables
- `gc_exstate()`: Handles collection of execution state
- `gc_tree()`: Handles collection of tree structures

## Execution Engine

The execution engine is implemented in `engine.c` with the core function `tengine()`. It uses a register-based virtual machine approach with the following key registers:

- `pc`: Program counter
- `op`: Current opcode
- `areg`: Argument registers
- `yreg`: Current environment
- `andb`: Current and-box (for concurrent execution)
- `chb`: Current choice-box (for backtracking)

The engine implements a threaded code interpreter on platforms that support it, which provides better performance than a traditional switch-based interpreter.

## Concurrency Model

AKL's concurrency model is based on the concept of and/or-parallel execution:

1. **And-Boxes**: Represent concurrent computations (implemented in `andbox` structures)
2. **Choice-Boxes**: Represent alternative computations (implemented in `choicebox` structures)
3. **Guards**: Control execution based on constraint satisfaction

The implementation uses a tree structure to represent the computation state, with and-boxes and choice-boxes as nodes.

## Constraint System

The constraint system is built around variables and their domains:

1. **Unbound Variables**: Represented with the UVA tag
2. **Constrained Variables**: Represented with the GVA tag
3. **Finite Domain Constraints**: Implemented for integer variables
4. **Equality Constraints**: Implemented over rational trees

## System Architecture Assumptions

The implementation makes several assumptions about the underlying system architecture:

1. **Word Size**: Primarily designed for 32-bit architectures, with accommodations for 64-bit (Alpha)
2. **Pointer Alignment**: Assumes specific alignment requirements (e.g., double-word alignment for atoms)
3. **Address Space**: Uses `PTR_ORG` to handle address space limitations on some architectures
4. **Endianness**: Implicitly assumes a specific byte order in some operations

## Conclusion

The AKL implementation represents a sophisticated system that combines efficient memory representation through tagged pointers with a concurrent constraint programming model. The implementation makes clever use of bit manipulation and pointer arithmetic to achieve efficiency, but also relies on specific assumptions about the underlying hardware architecture that may not hold on modern systems.

This specification provides a foundation for understanding the core mechanisms of the AKL implementation, particularly its tagged pointer scheme and memory management system, which are critical to its operation.
