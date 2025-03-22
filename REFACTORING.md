# AKL Implementation Refactoring Plan

This document outlines preliminary thoughts and strategies for refactoring the AKL (Agents Kernel Language) implementation to ensure it can be built and executed on modern Unix-type machines.

## Current State Assessment

The AKL implementation was last runnable on Sun workstations in the 1990s and faces several challenges when attempting to build and run on modern systems:

1. **32-bit Architecture Assumptions**: The code uses a tagged pointer scheme that assumes 32-bit architecture with specific bit layouts.

2. **Outdated Build System**: The Autoconf-based build system is from an early version and lacks modern features and compatibility.

3. **Dependency on Old Libraries**: The system depends on specific versions of libraries like GMP that may not be compatible with modern systems.

4. **Platform-Specific Code**: Contains code specific to Sun SPARC and HP architectures that may not work on modern x86/x86_64 systems.

5. **C Language Standards**: Uses older C language constructs and may not compile with modern C compilers and their stricter standards enforcement.

## Refactoring Goals

1. **Ensure Portability**: Make the code buildable and runnable on modern 64-bit Unix-like systems (Linux, macOS, BSD).

2. **Preserve Functionality**: Maintain the original semantics and behavior of the AKL implementation.

3. **Modernize Build System**: Update the build system to use modern tools and practices.

4. **Improve Maintainability**: Refactor code to improve readability and maintainability.

## Proposed Refactoring Approach

### 1. Build System Modernization

1. **Replace Autoconf/Automake**: 
   - Consider replacing with CMake or a simpler Makefile-based system
   - Update configuration scripts to detect modern systems correctly
   - Simplify the build process to reduce dependencies

2. **Dependency Management**:
   - Update GMP integration to work with modern versions
   - Consider using package managers or submodules for dependencies
   - Add proper version checking for required tools and libraries

3. **Installation Process**:
   - Modernize installation paths to follow current FHS standards
   - Update scripts to handle modern filesystem layouts

### 2. Code Modernization

1. **Tagged Pointer Scheme**:
   - Refactor the tagged pointer implementation to work on both 32-bit and 64-bit architectures
   - Modify bit manipulation code to be architecture-independent
   - Consider using C99/C11 features like `stdint.h` types for better portability

2. **Memory Management**:
   - Review and update the garbage collection system for 64-bit compatibility
   - Address potential alignment issues on modern architectures
   - Consider memory allocation strategies that work well with modern memory hierarchies

3. **Platform-Specific Code**:
   - Identify and refactor platform-specific code sections
   - Create abstraction layers for architecture-dependent operations
   - Implement proper feature detection instead of hardcoded platform checks

4. **Compiler Compatibility**:
   - Address warnings and errors from modern compilers
   - Fix deprecated function usage
   - Ensure proper function prototypes and type declarations

### 3. Language Processing Components

1. **Parser Updates**:
   - Update Bison/Flex usage to modern versions
   - Address any deprecated features in parser generation

2. **Compiler Pipeline**:
   - Review and update the AKL compiler stages for modern compatibility
   - Ensure the bootstrap process works on modern systems

3. **Foreign Function Interface**:
   - Update the foreign function interface to work with modern C calling conventions
   - Consider supporting additional foreign languages

### 4. Testing and Verification

1. **Test Suite**:
   - Develop or recover test cases to verify correct behavior
   - Create automated tests for different components
   - Implement regression testing to ensure refactoring doesn't break functionality

2. **Benchmarking**:
   - Create performance benchmarks to compare before and after refactoring
   - Identify performance bottlenecks on modern hardware

## Implementation Strategy

1. **Incremental Approach**:
   - Start with making minimal changes to get the system building
   - Gradually refactor components while maintaining functionality
   - Use version control to track changes and allow for rollbacks

2. **Documentation**:
   - Document all changes and the reasoning behind them
   - Update user and developer documentation
   - Create architecture documentation for the refactored system

3. **Compatibility Layer**:
   - Consider implementing a compatibility layer for critical components
   - Allow configuration options to choose between original and refactored implementations

## Specific Technical Challenges

### 1. Tagged Pointer Implementation

The current implementation uses low bits of pointers for type tags and a high bit for GC marking. On 64-bit systems:

- More bits are available for tagging
- Alignment requirements may differ
- Pointer arithmetic needs adjustment

Proposed solution: Create a new `pointer.h` that defines architecture-independent macros for pointer manipulation, with implementations for both 32-bit and 64-bit systems.

### 2. Garbage Collection

The current GC assumes 32-bit address space and may not scale well to 64-bit:

- Scanning larger heaps efficiently
- Managing larger address spaces
- Handling modern memory hierarchies

Proposed solution: Review and update the GC algorithm, possibly implementing generational collection or other modern techniques.

### 3. Concurrency Model

The original implementation's concurrency model may need updates for modern multi-core systems:

- Thread safety considerations
- Efficient synchronization primitives
- Leveraging multiple cores effectively

Proposed solution: Review the concurrency model and update it to use modern threading libraries and synchronization primitives.

## Next Steps

1. **Create a Working Build Environment**:
   - Set up a controlled build environment for testing
   - Document the current build process in detail
   - Identify immediate blockers to compilation

2. **Minimal Viable Build**:
   - Make minimal changes to get the system to compile
   - Focus on critical path components first
   - Document all changes made

3. **Test Basic Functionality**:
   - Run simple AKL programs to verify basic functionality
   - Identify and fix runtime issues
   - Create a baseline for further refactoring

4. **Detailed Refactoring Plan**:
   - Develop a more detailed plan for each component
   - Prioritize changes based on impact and difficulty
   - Create milestones and success criteria

## Conclusion

Refactoring the AKL implementation for modern systems presents significant challenges but is feasible with a systematic approach. By focusing on architecture independence, build system modernization, and incremental improvements, we can preserve this valuable implementation while making it accessible to modern developers and researchers.
