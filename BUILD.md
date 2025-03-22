# AKL (Agents Kernel Language) Build System Documentation

This document describes the build system for the AKL (Agents Kernel Language) implementation, focusing on how the project is built, configured, and installed.

## Overview

The AKL implementation uses a combination of GNU Autoconf-based configuration and custom build scripts. The system was designed for Unix-like environments, particularly Sun workstations from the 1990s.

## Build System Components

### Configuration Scripts

1. **configure.in**: The Autoconf input file used to generate the `configure` script.
   - Handles platform-specific configurations
   - Configures GMP (GNU Multiple Precision Arithmetic Library) integration
   - Sets up compiler flags and math library dependencies
   - Generates the main Makefile from Makefile.in

2. **configure**: The generated Autoconf script that:
   - Detects the host system type using `config.guess` and `config.sub`
   - Configures environment-specific settings
   - Allows disabling GMP with `--without-gmp`
   - Sets up compiler flags based on the detected environment

3. **mkbuilddirs**: A shell script that creates a "shadow" directory structure with symbolic links to source files, enabling out-of-source builds.

4. **mkinstalldirs**: A shell script that creates directory hierarchies needed for installation.

### Makefiles

1. **Makefile.in**: The template for the main Makefile, which defines:
   - Installation paths (`prefix`, `bindir`, `libdir`, etc.)
   - Build targets (`all`, `install`, `clean`, `dist`, etc.)
   - Subdirectory builds (emulator, compiler, library, etc.)
   - Documentation generation

2. **emulator/Makefile**: Builds the AKL emulator (virtual machine)
   - Compiles C source files
   - Generates parser code using Bison and Flex
   - Links with GMP and math libraries

3. **compiler/Makefile**: Builds the AKL compiler
   - Compiles AKL source files into PAM (Prolog Abstract Machine) files
   - Uses the AKL interpreter itself during compilation

4. **gmp/Makefile**: Builds the GMP library, which is used for arbitrary precision arithmetic
   - Configured for the specific host architecture
   - Built with libtool for shared and static library support

### Main Build Process

The build process follows these steps:

1. **Configuration**: The `configure` script is run to detect the system environment and generate appropriate Makefiles.

2. **Compilation**:
   - The GMP library is built first (unless `--without-gmp` is specified)
   - The emulator is compiled from C source files
   - The compiler is built using the AKL language itself
   - Additional libraries and tools are compiled

3. **Installation**:
   - Executables are installed to `$(bindir)`
   - Libraries to `$(libdir)`
   - Documentation to `$(infodir)`

## Build Dependencies

The AKL implementation requires:

1. **C Compiler**: GCC is preferred, but other compilers may work
2. **Bison**: For parser generation
3. **Flex**: For lexical analyzer generation
4. **M4**: For macro processing
5. **GMP**: For arbitrary precision arithmetic (can be disabled)
6. **Make**: GNU Make is preferred, especially on HP systems

## Build Targets

The main Makefile provides the following targets:

- **all**: Build the complete AKL system
- **install**: Install the AKL system in the configured directories
- **clean**: Remove files created by the build process
- **dist**: Create a distribution archive
- **make-gmp**: Build the GMP library
- **install-gmp**: Install the GMP library

## Environment Variables

The build system uses several environment variables:

- **CC**: C compiler command
- **CFLAGS**: C compiler flags
- **LDFLAGS**: Linker flags
- **AGENTSFLAGS**: Flags specific to the AKL implementation

## The `agents` Script

The `agents.in` file is a template for the main `agents` script, which:

1. Serves as the main entry point for running AKL programs
2. Handles compilation of AKL source files to PAM files
3. Processes foreign language interface declarations
4. Links C code with the AKL emulator
5. Manages temporary files and compilation steps

## Building Custom AKL Applications

To build a custom AKL application:

1. Write AKL source files (*.akl)
2. Optionally write foreign language interface files (*.fd)
3. Use the `agents` script to compile and link everything
4. The output is an executable that includes the AKL emulator and compiled code

## Platform-Specific Considerations

The build system includes special handling for:

- **HP-UX**: Special math library handling for HP 9000 systems
- **Sun Solaris**: Default target platform with optimized settings
- **Other Unix systems**: Basic support with potential limitations

## Troubleshooting

Common build issues include:

1. Missing dependencies (Bison, Flex, GMP)
2. Platform-specific math library issues
3. Compiler version incompatibilities
4. Path and environment variable issues
