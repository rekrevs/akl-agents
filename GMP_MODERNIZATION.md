# GMP Library Modernization Strategy

## Overview

The AKL implementation currently includes an old version of the GNU Multiple Precision Arithmetic Library (GMP) from the early 1990s. This document outlines the strategy for replacing it with the modern official GMP library.

## Current GMP Integration

The bundled GMP library is integrated into the AKL implementation in the following ways:

1. **Direct inclusion**: The GMP source code is included in the `gmp/` directory
2. **Custom memory management**: The AKL implementation provides custom memory allocation functions for GMP
3. **API usage**: The emulator uses GMP functions for bignum operations
4. **Build system**: The build system compiles the bundled GMP library as part of the AKL build

## Compatibility Considerations

When replacing the old GMP with the modern version, we need to consider:

1. **API changes**: The GMP API has evolved since the early 1990s
2. **Memory management**: Modern GMP has different memory management interfaces
3. **Type definitions**: Changes in type definitions and structures
4. **Custom functions**: The AKL implementation uses some custom functions not in the standard GMP

## Modernization Strategy

### 1. Dependency Management

Replace the bundled GMP with a dependency on the modern GMP library:

- Use the CMake build system to find and link against the system GMP
- Document the required GMP version (recommend 6.0.0 or later)
- Provide instructions for installing GMP on different platforms

### 2. Adapter Layer

Create an adapter layer to handle API differences:

- Implement a `gmp_compat.h` header with compatibility macros and functions
- Map old function names to new ones where needed
- Implement any missing functions that were custom in the old version

### 3. Memory Management

Update the memory management integration:

- Adapt the custom allocator functions to the modern GMP interface
- Ensure proper garbage collection integration
- Test memory usage patterns to avoid leaks

### 4. Testing Strategy

Develop tests to verify correct operation:

- Create unit tests for bignum operations
- Compare results between old and new implementations
- Test edge cases and performance

## Implementation Plan

1. **Analysis Phase**
   - Document all GMP function calls in the codebase
   - Identify API differences between old and modern GMP
   - Map old functions to their modern equivalents

2. **Adapter Implementation**
   - Create the compatibility layer
   - Implement missing functions

3. **Integration**
   - Update build system to use external GMP
   - Modify code to use the adapter layer
   - Remove the bundled GMP code

4. **Testing**
   - Develop and run test cases
   - Verify correctness of arithmetic operations
   - Check for memory leaks

5. **Documentation**
   - Update build instructions
   - Document any behavioral changes

## Specific API Changes to Address

Based on initial analysis, these are some of the key differences to handle:

1. **Function naming**: Modern GMP uses a more consistent naming scheme
2. **Type definitions**: Changes in MP_INT structure and related types
3. **Memory functions**: Different allocation and deallocation interfaces
4. **Missing functions**: Some functions like `mpz_xor_kludge` were custom and need to be reimplemented

## Conclusion

Replacing the bundled GMP with the modern version will improve maintainability, performance, and security. The adapter layer approach allows us to make this change without extensive modifications to the core AKL code, following our incremental modernization strategy.
