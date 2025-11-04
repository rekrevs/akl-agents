# Build System Analysis and Modernization Requirements
## AGENTS v0.9 Porting Study - Phase 3, Document 6

**Version:** 1.0
**Date:** 2025-11-04
**Status:** Complete

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Current Build System Overview](#current-build-system-overview)
3. [Vintage Build Tools Analysis](#vintage-build-tools-analysis)
4. [Compiler Requirements](#compiler-requirements)
5. [Library Dependencies](#library-dependencies)
6. [Deprecated Functions and APIs](#deprecated-functions-and-apis)
7. [Build Process Dependencies](#build-process-dependencies)
8. [Modernization Requirements](#modernization-requirements)
9. [Bootstrap Strategies](#bootstrap-strategies)
10. [Platform-Specific Build Issues](#platform-specific-build-issues)
11. [Recommendations](#recommendations)

---

## 1. Executive Summary

The AGENTS v0.9 build system is based on **early 1990s technology** and presents significant challenges for building on modern systems:

**Critical Findings:**

1. **Autoconf vintage:** configure.in uses autoconf 1.x syntax (circa 1992-1994)
2. **Build utilities:** config.guess and config.sub are from 1991-1993
3. **Compiler dependency:** Requires gcc 2.0+ for critical optimizations
4. **Deprecated functions:** Uses getwd(), gcvt(), and potentially others
5. **Shell dependency:** Build scripts use `/bin/csh` (C shell)
6. **Documentation tools:** Requires bison, flex, m4, makeinfo, latex, bibtex
7. **Testing framework:** Uses DejaGnu test framework

**Key Challenge:** Over **30 years** have passed since this build system was created. Many assumptions about tool versions, library functions, and system behavior are outdated.

**Recommended Approach:** Multi-phase bootstrap strategy, potentially using architecture emulation (QEMU) to validate in a "known good" vintage environment before full modernization.

---

## 2. Current Build System Overview

### 2.1 Build System Architecture

```
configure.in  ────[autoconf]───>  configure
                                      │
                                      ├──> Makefile (from Makefile.in)
                                      ├──> config.status
                                      └──> config.h (indirectly)

Makefile  ────[make all]───>  Build sequence:
                               1. gmp (optional)
                               2. emulator (C → agents.o, oagents)
                               3. compiler (AKL → .pam)
                               4. environment (AKL → .pam)
                               5. version.pam
                               6. agents script
                               7. library packages
                               8. tools
                               9. demos
```

### 2.2 Configuration Process

**File:** `configure.in` (139 lines)

```bash
# Key steps in configure.in:

1. AC_INIT(agents.in)           # Initialize with agents.in
2. Host detection:
   - Manual parsing of --host=* argument
   - config.guess for automatic detection
   - config.sub for validation/expansion

3. GMP handling:
   - --without-gmp option
   - Sets CFLAGS_GMP, LDFLAGS_GMP, AGENTSFLAGS_GMP

4. Compiler detection:
   - AC_PROG_CC (finds C compiler)
   - Sets CFLAGS=-O2 for GCC

5. Install program detection:
   - AC_PROG_INSTALL

6. Math library handling:
   - Platform-specific AGENTSFLAGS_MATH
   - Platform-specific LDFLAGS_MATH
   - HP-UX special handling for asinh/acosh/atanh

7. Make detection:
   - AC_PROGRAM_CHECK(MAKE,gmake,gmake,make)
   - Prefers gmake on HP systems

8. Output generation:
   - AC_OUTPUT(Makefile)
```

### 2.3 Makefile Structure

**Top-level Makefile.in** (509 lines)

Key variables:
```makefile
VERSION = 0.9
SHELL = /bin/sh

# Tool definitions
BISON = bison
CC = @CC@
CFLAGS = @CFLAGS@
FLEX = flex
FLEXFLAGS = -8
M4 = m4
MAKE = @MAKE@
MAKEINFO = makeinfo
MAKEINFOFLAGS = --fill-column=70
RUNTEST = runtest

# Compiler flags variants
EAGENTSFLAGS = -DMETERING -DNDEBUG -DSTRUCT_ENV  # Environment field
OAGENTSFLAGS = -DMETERING -DNDEBUG               # Ordinary
SAGENTSFLAGS = -DNDEBUG -I$(srcdir)/emulator     # Script
TAGENTSFLAGS = -DMETERING -DNDEBUG -DTRACE -g    # Trace
```

**Emulator Makefile** (155 lines)

```makefile
BISON = bison
CC = gcc
CFLAGS = -g -I../gmp
FLEX = flex
FLEXFLAGS = -8
LDFLAGS = -L../gmp -lgmp -lm
M4 = gm4                # Note: GNU m4 assumed

# Build dependencies
agents: agents.o foreign_stub.o
	${CC} $(CFLAGS) $(AGENTSFLAGS) agents.o foreign_stub.o $(LDFLAGS) -o agents

parser.tab.c parser.tab.h: parser.y
	$(BISON) -d $(BISONFLAGS) parser.y

parser.yy.c: parser.l parser.tab.h
	$(FLEX) $(FLEXFLAGS) parser.l
	mv lex.yy.c parser.yy.c

parser.y: parser.y.m4
	$(M4) $(M4FLAGS) parser.y.m4 > parser.y

display.c: display.c.m4
	$(M4) $(M4FLAGS) display.c.m4 > display.c
```

### 2.4 Installation Process

```
make install:
  1. Create directories via mkinstalldirs
  2. Install GMP (libgmp.a, headers)
  3. Install emulator (oagents.o → agents.o, oagents → agents.e, headers)
  4. Install compiler (.pam files)
  5. Install environment (.pam files)
  6. Install version.pam
  7. Install agents script (with path substitution)
  8. Install library packages
  9. Install tools
 10. Install demos
 11. Install man page (agents.1)
 12. Install info files
 13. Install emacs mode (akl.el, comint.el)
```

### 2.5 Agents Driver Script

**File:** `agents.in` (210 lines, C shell script)

**Critical issue:** Uses `/bin/csh -f` (C shell)

```csh
#!/bin/csh -f

set AGENTSEXE = "--AGENTSEXE--"    # Replaced by sed during make
set AGENTSOBJ = "--AGENTSOBJ--"

# Boot files
set BOOT = --ENVIRONMENTDIR--/boot.pam
set COMP = --COMPILERDIR--/comp.pam
set VERS = "--VERSION--"
set BASICBOOT = "-b $BOOT -b $COMP -b $VERS"

# ... complex argument parsing ...
# ... compilation of .akl files ...
# ... foreign interface code generation ...
# ... C compilation and linking ...
# ... execution or saving ...
```

**Problems:**
- `/bin/csh` may not be available on modern systems
- C shell syntax not compatible with bash/sh
- Relies on `find -newer` behavior
- Uses `>&!` syntax (csh-specific)

---

## 3. Vintage Build Tools Analysis

### 3.1 Config.guess and Config.sub

**Files:** `config.guess`, `config.sub`

**Versions identified:**

```bash
# config.guess
#!/bin/sh
# Copyright (C) 1992, 1993 Free Software Foundation, Inc.
# This script attempts to guess a canonical system name.

# config.sub
#!/bin/sh
# Configuration validation subroutine script, version 1.1.
# Copyright (C) 1991, 1992, 1993 Free Software Foundation, Inc.
```

**Age:** 32-34 years old (as of 2025)

**Known issues:**
- Does not recognize modern architectures (x86-64, ARM64, RISC-V)
- Does not recognize modern OS versions
- Uses obsolete CPU type names
- Missing modern vendor/manufacturer names

**Specific gaps:**
```bash
# Missing from config.guess (needs to add):
- x86_64-*-linux-gnu
- aarch64-*-linux-gnu
- arm64-apple-darwin*
- x86_64-apple-darwin*
- *-*-freebsd1[2-9]*
- *-*-netbsd[8-9]*
- *-*-openbsd[6-9]*

# config.sub similarly outdated
```

### 3.2 Autoconf Version

**configure.in syntax analysis:**

```bash
# Autoconf 1.x style (circa 1992-1994):
AC_INIT(agents.in)           # Old style, no AC_CONFIG_SRCDIR
AC_PROG_CC                   # Still valid
AC_PROG_INSTALL             # Still valid
AC_PROGRAM_CHECK            # OBSOLETE, replaced by AC_CHECK_PROG in autoconf 2.x
AC_OUTPUT(Makefile)         # Old style, no AC_CONFIG_FILES
AC_SUBST(...)              # Still valid

# Missing modern constructs:
- AC_PREREQ([version])
- AC_CONFIG_SRCDIR([unique-file])
- AC_CONFIG_HEADERS([config.h])
- AC_CONFIG_FILES([...])
- Modern quoting style with [[...]]
```

**Compatibility issues:**
- Modern autoconf (2.69+, 2.71+) warns about obsolete macros
- Some macros removed entirely in autoconf 2.70+
- Manual host detection bypasses modern AC_CANONICAL_HOST

### 3.3 Automake

**Status:** Not used

AGENTS uses raw Makefile.in templates, not automake. This is both a blessing (less dependency on automake vintage) and a curse (manual make rules can have portability issues).

### 3.4 Make Compatibility

**Issue:** Makefile syntax compatibility

```makefile
# Potential issues:

1. Variable assignment style:
   VERSION = 0.9              # Spaces around = (OK)

2. Shell variable access:
   if [[ ${GCC} ]] ; then     # Double brackets (bash-specific)

3. Backticks in rules:
   `date`                     # Old style, should use $(shell date)

4. ld -r usage:
   ld -r -o agents.o $(OBJ)   # Direct ld invocation (fragile)

5. .SUFFIXES rules:
   .c.o:                      # Old-style suffix rules
   $(CC) -c $(CFLAGS) ...
```

**GNU make assumed in places:**
- Recursive make invocations
- $(MAKE) variable
- Conditional variable assignment

### 3.5 Shell Script Compatibility

**Critical problem:** C shell dependency

```bash
# agents.in uses csh:
#!/bin/csh -f

foreach ARG ($argv)          # csh syntax
  switch ($ARG)              # csh syntax
    case *.akl               # csh syntax
      set AKLS = "$AKLS ..."  # csh syntax
      breaksw                # csh syntax
  endsw
end

# Modern systems:
- /bin/csh may not exist
- tcsh often replaces csh
- Most systems use bash/sh as default
```

---

## 4. Compiler Requirements

### 4.1 GCC Version Requirements

**Minimum version:** gcc 2.0

**Evidence:**

```c
// emulator/instructions.h
#ifndef NOTHREAD
#if defined(__GNUC__) && (__GNUC__ >= 2)
#define THREADED_CODE
#endif
#endif
```

**Implications:**

1. **Threaded code optimization** requires gcc 2.0+
   - Uses computed goto (labels as values)
   - `goto *(address)` syntax
   - ~20-30% performance improvement

2. **Hard register allocation** requires gcc with register asm
   ```c
   #if defined(__GNUC__) && defined(HARDREGS)
   #ifdef alpha
   #define REGISTER1 asm("$23")
   #define REGISTER2 asm("$22")
   // ...
   #endif
   #endif
   ```

3. **Inline assembly** (minimal use)
   ```c
   #ifdef __GNUC__
   // Some platform-specific optimizations
   #endif
   ```

### 4.2 Modern Compiler Compatibility

**GCC evolution:**

- **gcc 2.0:** Released 1992 - required for AGENTS
- **gcc 3.x:** Released 2001 - major ABI changes
- **gcc 4.x:** Released 2005 - improved optimization
- **gcc 5.x:** Released 2015 - C11 support by default
- **gcc 7.x:** Released 2017 - stricter warnings
- **gcc 11.x:** Released 2021 - current stable
- **gcc 13.x:** Released 2023 - latest

**Potential compatibility issues:**

1. **Implicit function declarations** (error in modern gcc):
   ```c
   // Old code might have:
   strcpy(s, str);    // No prototype, implicit int return

   // Modern gcc requires:
   #include <string.h>
   strcpy(s, str);
   ```

2. **K&R style function definitions** (deprecated):
   ```c
   // AGENTS uses old style:
   Atom make_atom(str, hashval)
       char *str;
       int hashval;
   { ... }

   // Modern style:
   Atom make_atom(char *str, int hashval)
   { ... }
   ```

3. **Pointer arithmetic warnings:**
   ```c
   // Strict aliasing, pointer signedness warnings
   // More aggressive type checking
   ```

4. **Register variables (deprecated in C17):**
   ```c
   register int write_mode REGISTER1;
   // register keyword ignored by modern compilers
   // register asm() still works but discouraged
   ```

### 4.3 Clang Compatibility

**Status:** Likely compatible but untested

**Concerns:**

1. **Computed goto support:** Clang supports this (labels as values)
2. **Register asm:** Clang supports but with different constraints
3. **GCC extensions:** Most are supported by clang
4. **Optimization differences:** Clang may optimize differently

**Testing needed:**
- Threaded code dispatch with clang
- Hard register allocation on x86-64 (which registers?)
- Performance comparison

---

## 5. Library Dependencies

### 5.1 Standard C Library

**Headers used:**

```c
// emulator/include.h and various sources
#include <stdio.h>
#include <stdlib.h>     // Not on sequent (conditional exclusion)
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
```

**Platform-specific exclusions:**

```c
// emulator/c.h
#ifndef sequent
#include <stdlib.h>
#endif
```

**macintosh-specific:**

```c
// emulator/include.h
#ifdef macintosh
#include "macintosh.h"
#endif
```

### 5.2 Math Library

**Standard functions:** Used extensively in `builtin.c`, `is.c`

```c
// Standard math.h functions
sin, cos, tan, asin, acos, atan, atan2
sinh, cosh, tanh
exp, log, log10, pow, sqrt
ceil, floor, fabs, fmod
```

**Extended math functions (conditional):**

```c
// asinh, acosh, atanh
#ifdef NOASINH
// These functions not available
// configure.in sets -DNOASINH on HP-PA, m68k-hp-hpux
#else
asinh(), acosh(), atanh()
#endif
```

**Platform notes from configure.in:**

```bash
# HP-UX issues (lines 87-124):
# - HP Series 300/400: asinh/acosh/atanh not supported
# - HP Series 700/800: PA1.1 library needed
# - Even with PA1.1 library, functions buggy!
#   Example: acosh(0.0) returns 0.0 instead of NaN
#            asinh(sinh(5.0)) fails
# Resolution: Set -DNOASINH on all HP-UX
```

**Modern status:**
- asinh/acosh/atanh are standard in C99
- All modern libm implementations support them

### 5.3 GMP Library

**Version:** Bundled GMP (appears to be GMP 2.x era based on directory structure)

**Location:** `gmp/` subdirectory

**Integration:**
```makefile
CFLAGS_GMP = -I$(gmpdir)
LDFLAGS_GMP = -L$(gmpdir) -lgmp

# Can be excluded:
./configure --without-gmp
# Sets -DNOBIGNUM
```

**Modern GMP:**
- Current version: GMP 6.3.0 (2023)
- C99 compliant
- Better performance
- Better portability
- **Recommendation:** Replace bundled GMP with modern version

### 5.4 System Libraries

**Required:**
- libc (standard C library)
- libm (math library)
- libgmp (bundled or system)

**Optional:**
- Graphics Manager (InterViews C++) - mentioned in README
  - Requires C++ compiler
  - Requires InterViews system (1987-1989 era!)
  - **Likely broken on modern systems**

---

## 6. Deprecated Functions and APIs

### 6.1 getwd() - DEPRECATED

**File:** `emulator/foreign.c`

**Usage:**

```c
// Line 70:
extern char *getwd PROTO((char *pathname));

// Line 404:
getwd(cwd);

// Lines 386-395: Conditional implementation
#ifdef HAS_NO_GETWD
#ifdef SYS5
char *getwd(buf)
    char *buf;
{
  return getcwd(buf, MAXPATHLEN);
}
#endif
#endif
```

**Issue:**
- `getwd()` deprecated in POSIX.1-2001
- Removed in POSIX.1-2008
- Dangerous: no buffer length parameter

**Modern replacement:**
```c
char *getcwd(char *buf, size_t size);
```

**Fix required:**
```c
// Change:
getwd(cwd);

// To:
getcwd(cwd, sizeof(cwd));
```

### 6.2 gcvt() - DEPRECATED

**File:** `emulator/builtin.c`

**Usage:**

```c
// Line 26:
extern char *gcvt PROTO((double, int, char *));

// Lines 1510, 1530, 1537: Conditional use
#ifndef HAS_NO_GCVT
  gcvt(f, 15, str=cbuf);
#else
  sprintf(str=cbuf, "%.15g", f);  /* gcvt broken */
#endif
```

**Issue:**
- `gcvt()` was never standardized
- Marked obsolete in SVID 3
- Not in modern glibc
- Already has fallback to sprintf

**Modern replacement:**
```c
snprintf(cbuf, sizeof(cbuf), "%.15g", f);
```

**Fix required:**
- Remove `gcvt()` usage entirely
- Use `snprintf()` everywhere (already implemented as fallback)
- Remove HAS_NO_GCVT conditional compilation

### 6.3 ecvt() - DEPRECATED

**File:** `emulator/builtin.c`

**Usage:**

```c
// Line 25:
extern char *ecvt PROTO((double, int, int *, int *));

// Usage not found in current codebase (declaration only)
```

**Issue:**
- `ecvt()` deprecated like `gcvt()`
- Likely dead code

**Fix required:**
- Remove declaration if unused

### 6.4 Other Potential Issues

**Checked and NOT FOUND:**

- `index()`, `rindex()` - NOT USED (checked, only variable names)
- `bcopy()`, `bzero()` - NOT USED
- `gets()` - NOT USED (uses `fgets()`)

**Good news:** Most dangerous deprecated functions are not used.

---

## 7. Build Process Dependencies

### 7.1 Bison (Parser Generator)

**File:** `emulator/parser.y` (generated from `parser.y.m4`)

**Version requirements:** Unknown, likely bison 1.x era

**Usage:**
```makefile
parser.tab.c parser.tab.h: parser.y
	$(BISON) -d $(BISONFLAGS) parser.y
```

**Modern bison:**
- Current version: Bison 3.8.2 (2021)
- Should be backward compatible
- May generate different code
- May produce new warnings

**Potential issues:**
1. Deprecated `%pure_parser` (use `%define api.pure`)
2. Different warning levels
3. Generated code style differences

### 7.2 Flex (Lexer Generator)

**File:** `emulator/parser.l`

**Usage:**
```makefile
FLEX = flex
FLEXFLAGS = -8

parser.yy.c: parser.l parser.tab.h
	$(FLEX) $(FLEXFLAGS) parser.l
	mv lex.yy.c parser.yy.c
```

**Flag `-8`:** Generate 8-bit scanner (vs. 7-bit default in old flex)

**Modern flex:**
- Current version: Flex 2.6.4 (2017)
- 8-bit is default now
- `-8` flag deprecated but still accepted
- Better Unicode support available

**Potential issues:**
1. Different optimizations
2. Different warnings
3. Generated code differences

### 7.3 M4 (Macro Processor)

**Files:** `parser.y.m4`, `display.c.m4`

**Usage:**
```makefile
M4 = m4            # or gm4 (GNU m4)

parser.y: parser.y.m4
	$(M4) $(M4FLAGS) parser.y.m4 > parser.y

display.c: display.c.m4
	$(M4) $(M4FLAGS) display.c.m4 > display.c
```

**Note:** Emulator Makefile uses `M4 = gm4` (GNU m4 explicitly)

**Modern m4:**
- Current version: GNU m4 1.4.19 (2021)
- Should be fully compatible
- Uses may be simple (basic macro expansion)

**Potential issues:**
- Different default quote characters?
- Different builtin behavior?
- Need to examine .m4 files for specifics

### 7.4 Makeinfo (Texinfo Documentation)

**Files:** `doc/*.texinfo` (presumably)

**Usage:**
```makefile
MAKEINFO = makeinfo
MAKEINFOFLAGS = --fill-column=70

# From doc/Makefile (not shown):
makeinfo $(MAKEINFOFLAGS) agents-user.texinfo
```

**Modern makeinfo:**
- Part of Texinfo 7.0+ (2023)
- Should be backward compatible
- May produce better output

### 7.5 LaTeX, BibTeX, TeX (Documentation)

**From Makefile.in comments:**

```
#   dvi		Build .dvi files in the doc directory.  REQUIRES latex, bibtex,
#		tex, AND texindex.
#   ps		Build .ps files in the doc directory.  REQUIRES latex, bibtex,
#		tex, texindex, AND dvips.
```

**Modern status:**
- TeXLive 2023+ available
- Should work if .tex sources are compatible
- May need adjustments for modern LaTeX packages

### 7.6 DejaGnu (Testing Framework)

**Usage:**
```makefile
RUNTEST = runtest

check:
	(cd tests; \
	 $(RUNTEST) $(RUNTESTFLAGS) --tool agents AGENTS=../agents)
```

**Issue:**
- DejaGnu is quite old (based on Expect/Tcl)
- Current version: DejaGnu 1.6.3 (2021)
- Still maintained but archaic

**Modern alternative:**
- Consider migrating to modern test framework
- Python unittest, pytest
- Custom shell-based tests
- TAP (Test Anything Protocol)

### 7.7 Etags (Emacs Tags)

**Usage:**
```makefile
ETAGS = etags

TAGS:
	$(ETAGS) $(ETAGSFLAGS) *.c *.h
```

**Status:** Part of Emacs, still maintained

---

## 8. Modernization Requirements

### 8.1 Critical Replacements

**Priority 1: Immediate failures on modern systems**

1. **Update config.guess and config.sub**
   - Source: https://git.savannah.gnu.org/gitweb/?p=config.git
   - Current versions: 2023-01-01 or later
   - Add x86-64, ARM64, modern OS versions

2. **Fix getwd() → getcwd()**
   - One file: `emulator/foreign.c`
   - Simple search-and-replace
   - Add buffer size parameter

3. **Fix gcvt() → snprintf()**
   - One file: `emulator/builtin.c`
   - Use existing fallback code
   - Remove HAS_NO_GCVT conditional

4. **Convert agents.in from csh to bash**
   - Rewrite in POSIX sh or bash
   - Significant effort (~210 lines)
   - Complex logic (argument parsing, compilation, linking)

**Priority 2: Build system modernization**

5. **Update configure.in to autoconf 2.69+**
   - Replace AC_PROGRAM_CHECK with AC_CHECK_PROG
   - Add AC_PREREQ, AC_CONFIG_SRCDIR
   - Use AC_CONFIG_FILES instead of AC_OUTPUT
   - Modern quoting style

6. **Update Makefile.in for modern make**
   - Replace backticks with $(shell ...)
   - Fix double-bracket conditionals
   - Update ld -r usage (potential fragility)

**Priority 3: Code modernization**

7. **Convert K&R function definitions to ANSI C**
   - Many files: `config.c`, `builtin.c`, etc.
   - Example:
     ```c
     // Old:
     Atom make_atom(str, hashval)
         char *str;
         int hashval;
     { ... }

     // New:
     Atom make_atom(char *str, int hashval)
     { ... }
     ```

8. **Add missing #include directives**
   - Modern gcc strict about implicit declarations
   - May need string.h, stdlib.h, unistd.h in more places

9. **Fix compilation warnings**
   - Pointer signedness
   - Strict aliasing
   - Unused variables
   - Format string mismatches

**Priority 4: Dependency updates**

10. **Replace bundled GMP with modern version**
    - Option 1: Use system libgmp
    - Option 2: Update to GMP 6.3.0
    - Test bignum functionality thoroughly

11. **Remove Graphics Manager dependencies**
    - InterViews is dead (1987-1989)
    - Remove or replace if graphics needed
    - Likely not critical for core functionality

### 8.2 Testing Requirements

**After each modernization step:**

1. **Build test:**
   ```bash
   ./configure && make clean && make all
   ```

2. **Functionality test:**
   ```bash
   make check  # DejaGnu tests
   ```

3. **Regression test:**
   - Run demo programs
   - Compare output with baseline
   - Verify no behavioral changes

4. **Performance test:**
   - Benchmark before and after
   - Ensure modernization doesn't degrade performance
   - Especially critical for dispatch mechanism changes

---

## 9. Bootstrap Strategies

### 9.1 Overview of Strategies

**Goal:** Port AGENTS to modern x86-64 and ARM64 with **incremental, verifiable steps**.

Three primary strategies identified:

1. **Vintage Emulation Strategy** (RECOMMENDED)
2. **Direct Modernization Strategy**
3. **Hybrid Cross-Compilation Strategy**

### 9.2 Strategy 1: Vintage Emulation (RECOMMENDED)

**Concept:** Use architecture emulation (QEMU) to validate in vintage environment before modernization.

**Phase 1: Establish Baseline (Vintage Environment)**

```
Step 1.1: Set up QEMU emulation
  - Option A: QEMU user-mode for DEC Alpha
    - Alpha 64-bit port already exists and works
    - qemu-alpha-static with Debian/Alpha or Gentoo/Alpha

  - Option B: QEMU system-mode for SPARC
    - SPARC 32-bit was primary development platform
    - qemu-system-sparc with Debian SPARC or Solaris

  - Option C: Vintage x86 (32-bit)
    - qemu-system-i386 with Debian 3.1 (2005) or similar
    - Less ideal (no existing AGENTS port for x86)

Step 1.2: Build in vintage environment
  - Use period-appropriate gcc (2.95.x or early 3.x)
  - Use period-appropriate autoconf, make, bison, flex
  - Verify all tests pass
  - Create "golden" test output

Step 1.3: Document baseline behavior
  - Capture full test suite output
  - Document any quirks or issues
  - Create reference checksums for .pam files

Verification: AGENTS works in known-good environment
```

**Phase 2: Incremental Modernization (Still in Emulation)**

```
Step 2.1: Update build system components one at a time
  a. Update config.guess, config.sub
     - Verify: Build still works

  b. Update autoconf syntax (keeping autoconf 2.13 for now)
     - Verify: configure still generates correct Makefile

  c. Update Makefiles (remove obsolete syntax)
     - Verify: make all still works

  d. Convert agents.in from csh to bash
     - Verify: agents script still functions

Step 2.2: Update compiler and libraries incrementally
  a. Upgrade gcc 2.95 → 3.x
     - Verify: Threaded code still works
     - Verify: Hard registers still work
     - Verify: Performance within 5%

  b. Upgrade gcc 3.x → 4.x
     - Fix K&R function definitions
     - Fix implicit declarations
     - Verify: All tests pass

  c. Upgrade gcc 4.x → 7.x → 11.x
     - Fix new warnings
     - Verify: All tests pass

Step 2.3: Modernize deprecated functions
  a. Replace getwd() → getcwd()
  b. Replace gcvt() → snprintf()
  c. Verify: Exact same behavior

Step 2.4: Update GMP library
  a. Replace bundled GMP 2.x with GMP 6.3.0
  b. Verify: Bignum tests still pass
  c. Verify: Performance same or better

Verification: AGENTS works with modern tools in emulated environment
```

**Phase 3: Port to Modern Architecture (Exit Emulation)**

```
Step 3.1: Create x86-64 port configuration
  a. Study Alpha 64-bit port (already 64-bit)
  b. Create emulator/sysdeps.h entry for x86-64
     - Set TADBITS=64
     - Set PTR_ORG=0
     - Determine hard register allocation

  c. Build on x86-64 Linux with modern gcc
     - Disable HARDREGS initially
     - Enable THREADED_CODE

  d. Test without hard register optimization
     - Run full test suite
     - Verify correctness

Step 3.2: Optimize for x86-64
  a. Implement hard register allocation
     - Study gcc register constraints for x86-64
     - Allocate: pc, op, areg, yreg, andb, write_mode
     - Candidate registers: rbx, r12, r13, r14, r15

  b. Benchmark with and without HARDREGS

  c. Tune for x86-64 cache characteristics

Step 3.3: Create ARM64 port configuration
  a. Use x86-64 port as template
  b. Create emulator/sysdeps.h entry for ARM64
     - Set TADBITS=64
     - Set PTR_ORG=0
     - Set OptionalWordAlign=1 (strict alignment)

  c. Determine hard register allocation for ARM64
     - ARM64 has 31 general-purpose registers
     - Candidate registers: x19-x28 (callee-saved)

  d. Test on ARM64 (Raspberry Pi, AWS Graviton, etc.)

Verification: AGENTS works on x86-64 and ARM64
```

**Advantages of Vintage Emulation Strategy:**

✅ **Low risk:** Start with known-working environment
✅ **Incremental:** Each step is verifiable
✅ **Debuggable:** Can compare old vs new at each step
✅ **Educational:** Learn system behavior in original context
✅ **Reversible:** Can roll back any step that fails

**Disadvantages:**

❌ **Time-consuming:** Setting up QEMU environment takes effort
❌ **Complexity:** Managing emulated environment
❌ **Dependency:** Need to find vintage OS images

**Estimated timeline:**
- Phase 1 (Baseline): 1-2 weeks
- Phase 2 (Modernization): 3-4 weeks
- Phase 3 (New architecture): 2-3 weeks
- **Total: 6-9 weeks**

### 9.3 Strategy 2: Direct Modernization

**Concept:** Fix all modern compatibility issues first, then port.

**Phase 1: Modernize Build System**

```
Step 1: Update all build tools at once
  - Update config.guess, config.sub
  - Update configure.in for autoconf 2.69+
  - Modernize Makefiles
  - Convert agents.in to bash

Step 2: Fix code for modern compilers
  - Replace getwd(), gcvt()
  - Convert K&R to ANSI C
  - Fix all warnings with gcc 11+
  - Add missing includes

Step 3: Update dependencies
  - Replace bundled GMP with modern version
  - Update documentation tools

Verification: Builds on modern system (but which architecture?)
```

**Phase 2: Port to 64-bit**

```
Step 1: Try building on x86-64
  - Start with existing configure behavior
  - Fix issues as they arise

Step 2: Create x86-64 port configuration
  - Similar to Phase 3 of Strategy 1

Step 3: Test and validate
```

**Advantages:**

✅ **Faster initially:** No emulator setup
✅ **Simpler:** Only deal with modern environment

**Disadvantages:**

❌ **Higher risk:** Many changes at once
❌ **Harder debugging:** If something breaks, hard to isolate cause
❌ **No baseline:** Can't verify against original behavior
❌ **Potential dead-ends:** May hit unsolvable problems

**Estimated timeline:** 4-8 weeks (but higher risk of failure)

### 9.4 Strategy 3: Hybrid Cross-Compilation

**Concept:** Build modern tools, but cross-compile for vintage architecture initially.

**Phase 1: Set up cross-compilation**

```
Step 1: Build cross-compiler for Alpha or SPARC
  - Use crosstool-ng or similar
  - Target: Alpha with modern gcc

Step 2: Cross-compile AGENTS for Alpha
  - Use modern build system on x86-64 host
  - Generate Alpha binaries
  - Test with QEMU user-mode

Step 3: Incrementally modernize
  - Similar to Strategy 1, but cross-compiling
```

**Advantages:**

✅ **Modern host tools:** Use modern Linux on x86-64
✅ **Vintage target verification:** Still test on Alpha/SPARC

**Disadvantages:**

❌ **Complex:** Cross-compilation adds another dimension
❌ **Toolchain issues:** Building cross-compiler can be difficult

**Estimated timeline:** 5-10 weeks

### 9.5 Recommended Strategy: Variant of Strategy 1

**Recommended approach:** **Vintage Emulation with QEMU Alpha**

**Why Alpha:**

1. **Already has 64-bit port** - proves system works on 64-bit
2. **TADBITS=64** - same as target x86-64/ARM64
3. **PTR_ORG=0** - same as target (unlike HP-PA, AIX)
4. **QEMU support** - qemu-alpha well-maintained
5. **Debian Alpha** - vintage Debian distros available

**Simplified phases:**

**Phase 0: Preparation**
- Set up QEMU Alpha environment (Debian Etch 4.0, circa 2007)
- Install gcc 4.1.x, autoconf 2.61, bison, flex, make
- Build AGENTS from pristine source
- Verify all tests pass → **BASELINE**

**Phase 1: Modernize in Alpha environment**
- Update build tools incrementally
- Upgrade gcc incrementally (4.1 → 4.7 → 7.x → 11.x)
- Replace deprecated functions
- Each step: build, test, compare to baseline

**Phase 2: Port to x86-64**
- Use modernized source from Phase 1
- Create x86-64 sysdeps.h configuration
- Build on native x86-64
- Allocate hard registers for x86-64
- Benchmark and optimize

**Phase 3: Port to ARM64**
- Use x86-64 port as template
- Create ARM64 sysdeps.h configuration
- Handle strict alignment (OptionalWordAlign=1)
- Allocate hard registers for ARM64
- Test on ARM64 hardware

**Verification at each phase:**
- ✅ Builds without errors
- ✅ All DejaGnu tests pass
- ✅ Demo programs run correctly
- ✅ Performance within acceptable range
- ✅ No memory corruption (valgrind)
- ✅ No undefined behavior (sanitizers)

---

## 10. Platform-Specific Build Issues

### 10.1 Modern Linux (x86-64)

**Likely issues:**

1. **Missing /bin/csh:**
   ```bash
   # Typical modern Linux:
   /bin/sh → bash
   /bin/bash exists
   /bin/csh → NOT INSTALLED
   ```
   **Fix:** Install tcsh or rewrite agents.in

2. **GCC strictness:**
   - Implicit declarations → error
   - K&R definitions → warning/error
   - Pointer signedness → warning

3. **Linker differences:**
   - `ld -r` behavior may differ
   - PIE (Position Independent Executable) by default
   - Stack protection by default

4. **Library paths:**
   - /usr/lib64 vs /usr/lib
   - Multilib issues

### 10.2 macOS (ARM64 - Apple Silicon)

**Likely issues:**

1. **No /bin/csh:**
   - macOS removed csh in recent versions
   - Need to rewrite agents.in

2. **Clang, not GCC:**
   - Clang is default compiler
   - Most gcc extensions work, but test needed

3. **System Integrity Protection:**
   - May interfere with dynamic loading

4. **Alignment:**
   - ARM64 requires strict alignment
   - Need OptionalWordAlign=1

5. **No libm:**
   - Math functions in libc on macOS
   - LDFLAGS_MATH should be empty or omitted

### 10.3 FreeBSD, OpenBSD, NetBSD

**Likely issues:**

1. **Different make:**
   - BSD make vs GNU make
   - Syntax incompatibilities
   - Need to use gmake

2. **Different install:**
   - BSD install vs GNU install
   - Different flags

3. **Library locations:**
   - /usr/local/lib vs /usr/lib
   - Need to set LDFLAGS

4. **tcsh vs csh:**
   - May have tcsh, not csh

---

## 11. Recommendations

### 11.1 Immediate Actions (Do First)

1. **Create modernization branch:**
   ```bash
   git checkout -b modernization
   ```

2. **Update config.guess and config.sub:**
   ```bash
   wget -O config.guess 'https://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.guess;hb=HEAD'
   wget -O config.sub 'https://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.sub;hb=HEAD'
   chmod +x config.guess config.sub
   ```

3. **Fix deprecated functions (easy wins):**
   ```bash
   # In emulator/foreign.c:
   sed -i 's/getwd(cwd)/getcwd(cwd, sizeof(cwd))/g' emulator/foreign.c

   # In emulator/builtin.c:
   # Remove gcvt() usage, use snprintf() fallback everywhere
   ```

4. **Test on modern system:**
   ```bash
   ./configure
   make 2>&1 | tee build.log
   # Analyze errors
   ```

### 11.2 Short-Term Goals (1-2 months)

1. **Set up QEMU Alpha environment**
   - Document setup process
   - Create reproducible VM image
   - Establish baseline

2. **Incremental modernization in Alpha**
   - Follow Phase 1 and Phase 2 of recommended strategy
   - Document each step
   - Create test checkpoints

3. **Convert agents.in to bash**
   - Rewrite 210-line csh script
   - Thoroughly test all argument parsing
   - Test compilation and linking workflows

4. **Update configure.in for modern autoconf**
   - Test with autoconf 2.69 and 2.71
   - Ensure backward compatibility

### 11.3 Medium-Term Goals (3-6 months)

1. **Complete modernization in Alpha environment**
   - Modern gcc (11.x+)
   - Modern GMP (6.3.0)
   - All deprecated functions replaced

2. **Port to x86-64**
   - Create sysdeps.h configuration
   - Implement hard register allocation
   - Comprehensive testing

3. **Port to ARM64**
   - Create sysdeps.h configuration
   - Handle alignment requirements
   - Test on multiple ARM64 platforms

### 11.4 Long-Term Goals (6-12 months)

1. **Comprehensive test suite**
   - Migrate from DejaGnu to modern framework
   - Add unit tests for emulator components
   - Add integration tests
   - Add performance benchmarks

2. **CI/CD pipeline**
   - Automated builds for x86-64, ARM64
   - Automated testing
   - Performance tracking

3. **Modern package distribution**
   - .deb packages for Debian/Ubuntu
   - .rpm packages for Fedora/RHEL
   - Homebrew formula for macOS
   - Docker containers

4. **Documentation updates**
   - Update installation instructions
   - Update build instructions
   - Add porting guide
   - Add troubleshooting guide

### 11.5 Risk Mitigation

**Risk 1: Emulator setup too difficult**

- **Mitigation:** Have fallback to direct modernization (Strategy 2)
- **Fallback trigger:** If emulator setup takes >2 weeks

**Risk 2: Modernization breaks functionality**

- **Mitigation:** Incremental steps with verification
- **Mitigation:** Keep baseline environment for comparison
- **Fallback:** Revert specific changes

**Risk 3: Hard register allocation doesn't work on x86-64**

- **Mitigation:** HARDREGS is optional (performance feature)
- **Fallback:** Disable HARDREGS, rely on compiler optimization

**Risk 4: Performance degradation**

- **Mitigation:** Benchmark at each step
- **Mitigation:** Profile to identify bottlenecks
- **Fallback:** Use older gcc version if newer ones regress

**Risk 5: 64-bit addressing issues**

- **Mitigation:** Alpha port proves it works
- **Mitigation:** Extensive testing with large heaps
- **Mitigation:** Use sanitizers (AddressSanitizer, UndefinedBehaviorSanitizer)

### 11.6 Success Criteria

**Phase 1 Success (Vintage Baseline):**
- ✅ Builds in QEMU Alpha environment
- ✅ All tests pass
- ✅ Demo programs work
- ✅ Documented baseline behavior

**Phase 2 Success (Modernization):**
- ✅ Builds with gcc 11+ in QEMU Alpha
- ✅ All tests pass (same results as baseline)
- ✅ Modern tools (autoconf 2.69+, etc.)
- ✅ No deprecated functions

**Phase 3 Success (x86-64 Port):**
- ✅ Builds on native x86-64 Linux
- ✅ All tests pass
- ✅ Performance within 10% of Alpha baseline
- ✅ Hard register optimization works

**Phase 4 Success (ARM64 Port):**
- ✅ Builds on ARM64 (Linux and macOS)
- ✅ All tests pass
- ✅ Performance within 10% of x86-64
- ✅ Handles strict alignment correctly

---

## Conclusion

The AGENTS v0.9 build system is a **30+ year old artifact** requiring significant modernization. The **vintage emulation strategy** offers the lowest risk path to successful porting by:

1. **Establishing a verifiable baseline** in a known-good environment
2. **Incrementally modernizing** tools and code with continuous verification
3. **Porting to modern architectures** only after modernization is complete

**Key success factors:**

- Patience and discipline to follow incremental approach
- Thorough testing at each step
- Good documentation of process and findings
- Willingness to roll back and try alternatives if needed

**Estimated total timeline:** 6-9 months for complete port to x86-64 and ARM64

**Next steps:**

1. Create document 07-TESTING-STRATEGY.md (unit testing, integration testing)
2. Create document 08-RISK-ASSESSMENT.md (detailed risk analysis)
3. Create document 09-IMPLEMENTATION-ROADMAP.md (detailed step-by-step plan)
4. Set up QEMU Alpha environment
5. Begin Phase 0 (Baseline establishment)

---

**Document Status:** Complete
**Last Updated:** 2025-11-04
**Next Document:** 07-TESTING-STRATEGY.md
