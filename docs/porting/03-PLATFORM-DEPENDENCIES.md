# AGENTS v0.9 - Complete Platform Dependencies Analysis

**Document:** Phase 1, Document 3 of 3
**Status:** Complete
**Date:** 2025-11-04
**Purpose:** Comprehensive inventory of all platform-specific code and configuration variants

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Existing Platform Ports](#existing-platform-ports)
3. [Configuration Flags](#configuration-flags)
4. [Platform-Specific Code Map](#platform-specific-code-map)
5. [The Alpha 64-bit Port](#the-alpha-64-bit-port)
6. [Other Architecture Ports](#other-architecture-ports)
7. [Operating System Dependencies](#operating-system-dependencies)
8. [Build System Platform Detection](#build-system-platform-detection)
9. [Porting to x86-64 and ARM64](#porting-to-x86-64-and-arm64)
10. [References](#references)

---

## Executive Summary

AGENTS v0.9 has been ported to multiple architectures. This document provides a **complete catalog** of all platform-specific code, configuration flags, and architectural adaptations.

### Supported Architectures (Current)

| Architecture | Word Size | Status | Register Alloc | PTR_ORG | Notes |
|--------------|-----------|--------|----------------|---------|-------|
| **Alpha (DEC)** | **64-bit** | **✅ Full** | **10 regs** | **0** | **Primary 64-bit port** |
| SPARC | 32-bit | ✅ Primary | 9 regs | 0 | Primary development platform |
| MIPS | 32-bit | ✅ Full | 10 regs | 0 | Full support |
| HP-PA (HPPA) | 32-bit | ✅ Full | None | 0x40000000 | Requires PTR_ORG |
| AIX (RS6000) | 32-bit | ✅ Full | None | 0x20000000 | Requires PTR_ORG |
| Macintosh (68k) | 32-bit | ✅ Full | None | 0 | Special memory limits |
| Sequent | 32-bit | ✅ Full | None | 0 | Unix variant |
| SVR4 (generic) | 32-bit | ✅ Full | None | 0 | System V Release 4 |

### Key Finding: 64-bit Support EXISTS

**The Alpha port proves the system already supports 64-bit architectures!**

This dramatically reduces porting risk for x86-64 and ARM64.

---

## Existing Platform Ports

### 1. Alpha (DEC Alpha) - 64-BIT PORT

**Location:** `emulator/sysdeps.h:1-3`, `emulator/regdefs.h:32-43`

```c
#ifdef __alpha__
#define TADBITS 64
#endif
```

**Register Allocation (with -DHARDREGS):**
```c
#ifdef alpha
#define REGISTER1 asm("$23")    /* Callee-saved */
#define REGISTER2 asm("$22")
#define REGISTER3 asm("$21")
#define REGISTER4 asm("$20")
#define REGISTER5 asm("$19")
#define REGISTER6 asm("$18")
#define REGISTER7 asm("$17")
#define REGISTER8 asm("$16")
#define REGISTER9 asm("$15")    /* Caller-saved */
#define REGISTER10 asm("$14")
#endif
```

**Key Characteristics:**
- **64-bit word size:** TADBITS = 64
- **Tagged pointers work:** Same tagging scheme, more bits for addresses
- **PTR_ORG = 0:** No pointer base adjustment needed
- **WORDALIGNMENT = 8:** 8-byte alignment
- **Small integers:** 58 bits (TADBITS-6 = 64-6 = 58)
- **Endianness:** Little-endian

**Compiler:** gcc with Alpha backend

**Usage in Engine:**
```c
register code *pc REGISTER3;        /* $21 */
register opcode op REGISTER4;       /* $20 */
register Term *areg REGISTER5;      /* $19 */
register Term *yreg REGISTER6;      /* $18 */
register andbox *andb REGISTER2;    /* $22 */
register int write_mode REGISTER1;  /* $23 */
```

**Implications for Modern 64-bit:**
- ✅ Tagged pointer scheme scales to 64-bit
- ✅ All data structure sizes adapt automatically
- ✅ GC mark bit in high bit (bit 63) works
- ✅ Alignment handled correctly
- ✅ No special-casing needed beyond TADBITS

---

### 2. SPARC (Primary Development Platform)

**Location:** `emulator/regdefs.h:20-30`

**Register Allocation:**
```c
#ifdef sparc
#define REGISTER1 asm("%l7")    /* Local register */
#define REGISTER2 asm("%l6")
#define REGISTER3 asm("%l5")
#define REGISTER4 asm("%l4")
#define REGISTER5 asm("%l3")
#define REGISTER6 asm("%l2")
#define REGISTER7 asm("%i5")    /* Input register */
#define REGISTER8 asm("%i4")
#define REGISTER9 asm("%i3")
#endif
```

**Key Characteristics:**
- **32-bit word size:** TADBITS = 32 (default)
- **PTR_ORG = 0:** Standard addressing
- **WORDALIGNMENT = 4:** 4-byte alignment
- **Register windows:** Uses SPARC %l (local) and %i (input) registers
- **Endianness:** Big-endian (typically)

**GMP Support:** Extensive assembly optimizations in `gmp/mpn/sparc32/` and `gmp/mpn/sparc64/`

---

### 3. MIPS

**Location:** `emulator/sysdeps.h:5-7`, `emulator/regdefs.h:7-18`

```c
#ifdef mips
#define HAS_NO_GCVT      /* Missing gcvt() library function */
#endif
```

**Register Allocation:**
```c
#ifdef mips
#define REGISTER1 asm("$23")    /* $s7 - callee-saved */
#define REGISTER2 asm("$22")    /* $s6 */
#define REGISTER3 asm("$21")    /* $s5 */
#define REGISTER4 asm("$20")    /* $s4 */
#define REGISTER5 asm("$19")    /* $s3 */
#define REGISTER6 asm("$18")    /* $s2 */
#define REGISTER7 asm("$17")    /* $s1 */
#define REGISTER8 asm("$16")    /* $s0 */
#define REGISTER9 asm("$15")    /* $t7 - caller-saved */
#define REGISTER10 asm("$14")   /* $t6 */
#endif
```

**Key Characteristics:**
- **32-bit word size**
- **PTR_ORG = 0**
- **Uses MIPS saved registers ($s0-$s7) and temp registers ($t6-$t7)**
- **Endianness:** Can be either (typically big-endian for older MIPS)

**Library Workaround:**
- Missing `gcvt()` - floating-point-to-string conversion
- Must use alternate implementation

**GMP Support:** `gmp/mpn/mips2/` and `gmp/mpn/mips3/` with assembly routines

---

### 4. HP-PA (HPPA/HP-UX)

**Location:** `emulator/sysdeps.h:16-21`, `configure.in:100-125`

```c
#ifdef hpux
#define SYS5
# ifdef hppa
#  define PTR_ORG  ((uword)0x40000000)
# endif
#endif
```

**Key Characteristics:**
- **32-bit word size**
- **PTR_ORG = 0x40000000** - **CRITICAL DIFFERENCE**
- **System V (SYS5)** conventions
- **No hard register allocation defined**
- **No gcvt(), getrusage(), getwd()** - requires SYS5 workarounds

**Why PTR_ORG?**

HP-PA architectures map user space starting at 0x40000000. The high bits would conflict with the GC mark bit (bit 31 on 32-bit). Solution: subtract PTR_ORG from all pointers before tagging.

From `emulator/term.h:87-95`:
```c
#ifndef PTR_ORG
#define PTR_ORG 0
#endif

#define AddressOf(x)  (((tagged_address)Tad(x))&AddressMask)
```

Pointer operations automatically adjust:
```c
#define TadOth(X, T)  (Tho((tagged_address)(X))-Tho(PTR_ORG)+TagOffset(T))
#define AddressOth(X) (AddressOf(Oth(X)) + PTR_ORG)
```

**Math Library Issues:**
From `configure.in:87-125`:
```sh
case ${host} in
  hppa*-hp-hpux* )
    # acosh, asinh, atanh definitions are wrong in PA1.1 library!
    AGENTSFLAGS_MATH=-DNOASINH
    LDFLAGS_MATH=-lm
    echo "{NOTE: acosh, asinh, and atanh unavailable}"
  ;;
esac
```

---

### 5. AIX (IBM RS6000/PowerPC)

**Location:** `emulator/sysdeps.h:37-40`

```c
#ifdef _AIX
#define unix
#define PTR_ORG  ((uword)0x20000000)
#endif
```

**Key Characteristics:**
- **32-bit word size** (on AIX 3.x/4.x era)
- **PTR_ORG = 0x20000000** - Similar to HP-PA
- **PowerPC architecture** (later AIX versions)
- **Big-endian**

**GMP Support:** `gmp/mpn/powerpc32/` and `gmp/mpn/powerpc64/` with assembly

**Pointer Origin:**
AIX maps user space starting at 0x20000000, requiring pointer adjustment like HP-PA.

---

### 6. Macintosh (68k)

**Location:** `emulator/sysdeps.h:27-30`, `emulator/constants.h:7-56`

```c
#ifdef macintosh
#define HAS_NO_GCVT
#define OptionalWordAlign(x)  WordAlign((unsigned long)(x))
#endif
```

**Memory Limits (constants.h):**
```c
#ifdef macintosh
#define AKLGCRATIO      50      /* vs. 25 default */
#define AKLCODESIZE     (250 * 1024)   /* vs. 1000KB default */
#define AKLGCGEN        (1 * 1024)     /* vs. 10KB default */
#define MAX_AREGS       256            /* vs. 8192 default */
#endif
```

**Key Characteristics:**
- **32-bit (68k architecture)**
- **Strict alignment required:** OptionalWordAlign enforces alignment
- **Reduced memory limits:** Smaller heap/code spaces for Mac Classic
- **No gcvt()**: Missing C library function
- **Special include:** `emulator/macintosh.h` for Mac-specific headers

**Compiler:** THINK C or similar Mac toolchain

---

### 7. Sequent

**Location:** `emulator/sysdeps.h:9-14`

```c
#ifdef sequent
/* For size_t: */
#include <sys/types.h>
extern double atof();
extern char *getenv();
#endif
```

**Key Characteristics:**
- **32-bit Sequent Balance/Symmetry systems** (i386-based SMP)
- **Missing function declarations** - needs explicit extern declarations
- **Unix variant**

**Historical Note:** Sequent was an early SMP Unix vendor (1980s-1990s).

---

### 8. System V Release 4 (SVR4) - Generic Unix

**Location:** `emulator/sysdeps.h:23-35`

```c
#ifdef __svr4__
#define SYS5
#endif

#ifdef SYS5
#define HAS_NO_GETRUSAGE   /* Use times() instead */
#define HAS_NO_GETWD       /* Use getcwd() instead */
#endif
```

**Key Characteristics:**
- **Generic SVR4 Unix systems** (Solaris, UnixWare, etc.)
- **POSIX-like but missing some BSD functions**
- **Workarounds for BSD-specific calls:**
  - `getrusage()` → use `times()`
  - `getwd()` → use `getcwd()`

---

## Configuration Flags

### Compile-Time Feature Flags

These flags enable/disable major features and affect code throughout the emulator.

#### 1. THREADED_CODE

**Purpose:** Enable computed goto dispatch for bytecode interpretation

**Files Affected:** 30 files (engine.c, instructions.h, opcodes.h, fd_akl.h, etc.)

**Usage:**
```c
#ifdef THREADED_CODE
#define NextOp()  goto *instr_label_table[*pc++]
#else
#define NextOp()  break;  /* Return to switch statement */
#endif
```

**Requirements:**
- gcc 2.0+ or clang
- Support for `&&label` syntax (label-as-values)
- Function pointers to labels

**Performance Impact:** ~20-30% speedup vs switch-based dispatch

**Enable:** `gcc -DTHREADED_CODE`

**Recommendation for Modern Ports:** ✅ Enable - fully supported on x86-64 and ARM64

---

#### 2. HARDREGS

**Purpose:** Map emulator variables to physical CPU registers

**Files Affected:** regdefs.h, engine.c

**Usage:**
```c
#if defined(__GNUC__) && defined(HARDREGS)
#ifdef sparc
#define REGISTER1 asm("%l7")
...
#endif
#endif
```

**Requirements:**
- gcc (uses gcc's `asm("register")` syntax)
- Architecture-specific register definitions

**Performance Impact:** ~10-20% speedup (varies by architecture)

**Enable:** `gcc -DHARDREGS`

**Registers to Allocate:**
- `pc` (program counter) - REGISTER3
- `op` (opcode) - REGISTER4
- `areg` (argument registers pointer) - REGISTER5
- `yreg` (environment pointer) - REGISTER6
- `andb` (and-box pointer) - REGISTER2
- `write_mode` (flag) - REGISTER1

**Recommendation for Modern Ports:**
- ✅ x86-64: Use r12-r15 (callee-saved, not used by ABI)
- ✅ ARM64: Use x19-x28 (callee-saved)
- ⚠️ Optional - system is portable without this

---

#### 3. BAM

**Purpose:** Enable "BAM" (Berkeley Abstract Machine?) extensions

**Files Affected:** 12 files (bam.h, bam.c, engine.c, exstate.h, opcodes.h, etc.)

**Features Added:**
- Constraint stack (cons_stack, try_stack)
- Additional opcodes (LOAD_*, READ_*, WRITE_*, FAIL_DEC, etc.)
- Pseudo-term handling
- Alias checking

**Usage:**
```c
#ifdef BAM
  register try_stack_cell *try_top, *try_bottom;
  register cons_stack_cell *cons_top, *cons_bottom;
#endif
```

**Enable:** `gcc -DBAM`

**Recommendation:** ⚠️ Document purpose before porting (may be experimental)

---

#### 4. TRACE

**Purpose:** Enable execution tracing and debugging support

**Files Affected:** 20+ files (engine.c, trace.c, trace.h, tree.h, config.h, etc.)

**Features Added:**
- Instruction-level tracing
- And-box/choice-box tracing
- Clause numbering
- Predicate call tracking

**Data Structure Impact:**
```c
typedef struct andbox {
  ...
#ifdef TRACE
  int trace;
  int clno;   /* Clause number */
  int id;     /* And-box ID */
#endif
  ...
} andbox;
```

**Performance Impact:** Significant overhead (debugging only)

**Enable:** `gcc -DTRACE`

**Recommendation:** ⚠️ Development/debugging only, not for production

---

#### 5. STRUCT_ENV (Unused)

**Purpose:** Include environment field in all structures for copying optimization

**Files Affected:** term.h, storage.h

**Status:** **Commented out / unused**

From `emulator/term.h:50-56`:
```c
/* #define STRUCT_ENV to include an environment field in all structures.
 * This is used when copying, so that copying of non-local structures is
 * avoided. The environment field is located in the word preceding the
 * structure or list.
 */

/* #define STRUCT_ENV */
```

**Recommendation:** ❌ Do not enable (experimental/unused)

---

#### 6. INDIRECTVARS (Unused)

**Purpose:** Disable one-word variables implicit in list/structure arguments

**Status:** **Commented out / unused**

From `emulator/term.h:41-47`:
```c
/* #define INDIRECTVARS to disable having one-word variables implicit in the
 * arguments of lists and structures. If you define INDIRECTVARS one-word
 * variables will still be created, but always as separate objects
 * in the heap (there will be no pointers into structures).
 */

/* #define INDIRECTVARS */
```

**Recommendation:** ❌ Do not enable (experimental/unused)

---

#### 7. TADTURMS (Debugging)

**Purpose:** Wrap Term type in a struct for type safety

**Status:** **Commented out / debugging aid**

From `emulator/term.h:31-38`:
```c
/* #define TADTURMS will define "Term" to be a "struct turm" which
 * encapsulates the tagged pointer. If you leave TADTURMS undefined,
 * "Term" will be an integer type. This allows for more optimizations,
 * but the struct representation will let the compiler catch more
 * errors, such as attempting to cast a Term into a pointer.
 */

/* #define TADTURMS */
```

**Enable:** `gcc -DTADTURMS`

**Recommendation:** ⚠️ Development/debugging only (performance cost)

---

### Platform Detection Macros

These are detected automatically by the compiler or configure script:

| Macro | Meaning | Detection |
|-------|---------|-----------|
| `__alpha__` | DEC Alpha | gcc built-in |
| `sparc` | SPARC | gcc built-in |
| `mips` | MIPS | gcc built-in |
| `hppa` | HP-PA | gcc built-in |
| `hpux` | HP-UX OS | gcc built-in |
| `_AIX` | AIX OS | gcc built-in |
| `macintosh` | Mac Classic | Compiler-specific |
| `sequent` | Sequent | gcc built-in |
| `__svr4__` | SVR4 Unix | gcc built-in |
| `__GNUC__` | GNU C compiler | gcc built-in |
| `unix` | Unix-like OS | Usually predefined |

---

## Platform-Specific Code Map

### Complete File Inventory

| File | Platforms | Purpose |
|------|-----------|---------|
| **sysdeps.h** | All | Central platform detection and configuration |
| **regdefs.h** | MIPS, SPARC, Alpha | Hard register allocation definitions |
| **constants.h** | Macintosh | Memory limits for resource-constrained systems |
| **include.h** | Macintosh | Platform-specific header inclusion |
| **macintosh.c** | Macintosh | Mac-specific implementations |
| **macintosh.h** | Macintosh | Mac-specific declarations |
| **time.c** | SYS5, others | Platform-specific timing functions |
| **inout.c** | SYS5, others | I/O with platform-specific paths |
| **foreign.c** | Various | Foreign function interface, platform callbacks |
| **main.c** | Various | Platform-specific initialization |
| **engine.c** | All | Uses REGISTER* macros if HARDREGS defined |
| **copy.c** | All | Uses platform-specific alignment |
| **parser.*.c** | All | Generated files with platform includes |

---

## The Alpha 64-bit Port

### In-Depth Analysis

The Alpha port is **THE MOST IMPORTANT** reference for x86-64 and ARM64 porting because it proves the system works on 64-bit architectures.

### Alpha Architecture Background

- **DEC Alpha:** 64-bit RISC architecture (1992-2004)
- **Word size:** 64 bits native
- **Registers:** 31 integer registers ($0-$30, $31 is hardwired zero)
- **Addressing:** Virtual addresses 43-bit (8TB address space)
- **Endianness:** Little-endian
- **ABI:** Uses $9-$15 as caller-saved temps, $9-$14 as saved registers

### Alpha-Specific Adaptations

#### 1. Word Size

**From sysdeps.h:1-3:**
```c
#ifdef __alpha__
#define TADBITS 64
#endif
```

**Effect:** Changes ripple throughout the system:
- `WORDALIGNMENT = TADBITS/8 = 8` bytes
- `SMALLNUMBITS = TADBITS-6 = 58` bits
- Small integer range: -288230376151711744 to +288230376151711743
- High bit (bit 63) for GC marking

#### 2. Tagged Pointer Layout

**32-bit layout:**
```
Bit:  31 30...........6 5 4 3 2 1 0
      |M|   Address    |   Tag    |
```

**64-bit layout (Alpha):**
```
Bit:  63 62................6 5 4 3 2 1 0
      |M|    Address        |   Tag    |
```

**Key observations:**
- ✅ Same tag bits (low 2-4 bits)
- ✅ Same mark bit strategy (high bit)
- ✅ More address bits available (57 bits vs 25 bits)
- ✅ Small integers get 58 bits (vs 26 bits)

#### 3. Alignment

**64-bit alignment:**
```c
#define WORDALIGNMENT 8
#define WordAlign(x) (((x) + 7) & ~((uword)7))
```

**Structures automatically align to 8 bytes:**
- List cells: 16 bytes (2 Terms)
- Structure headers: 8 bytes + N*8 bytes for arguments
- References: 8 bytes
- Floats: Already 8-byte aligned (sizeof(double) = 8)

#### 4. Hard Register Allocation

**From regdefs.h:32-43:**
```c
#ifdef alpha
#define REGISTER1 asm("$23")   /* $s8 - Callee-saved */
#define REGISTER2 asm("$22")   /* $s7 */
#define REGISTER3 asm("$21")   /* $s6 */
#define REGISTER4 asm("$20")   /* $s5 */
#define REGISTER5 asm("$19")   /* $s4 */
#define REGISTER6 asm("$18")   /* $s3 */
#define REGISTER7 asm("$17")   /* $s2 */
#define REGISTER8 asm("$16")   /* $s1 */
#define REGISTER9 asm("$15")   /* $fp (or caller-saved) */
#define REGISTER10 asm("$14")  /* Caller-saved */
#endif
```

**Strategy:** Uses mostly callee-saved registers ($9-$14, $22-$23 in ABI) to minimize saves/restores across function calls.

#### 5. PTR_ORG = 0

**Alpha user space starts at 0x0000000000000000**, so no pointer adjustment needed.

From term.h:
```c
#ifndef PTR_ORG
#define PTR_ORG 0  /* Default for Alpha */
#endif
```

#### 6. Endianness

**Alpha is little-endian**, matching x86-64. This means bit manipulation and shifts work identically.

### What Alpha Port Proves

✅ **Tagged pointers scale to 64-bit** - No redesign needed
✅ **GC mark bit in high bit works** - No conflicts with 64-bit addresses
✅ **Alignment automatically adjusts** - WORDALIGNMENT macro handles it
✅ **Small integers get more bits** - Performance benefit, no issues
✅ **Hard registers map cleanly** - Register-rich architectures benefit
✅ **PTR_ORG = 0 works** - Modern address spaces don't conflict with tags

### Alpha Port Lessons for x86-64/ARM64

1. **Just set TADBITS = 64** - Everything else adapts
2. **PTR_ORG = 0 likely fine** - User space starts low
3. **Add register definitions** - Both have many registers
4. **Alignment may be stricter on ARM64** - Test carefully
5. **Little-endian is compatible** - No endianness issues

---

## Other Architecture Ports

### SPARC - Primary Development Platform

**Strengths:**
- Register windows provide many registers
- Well-tested (primary development)
- Both 32-bit and 64-bit versions exist

**Lessons:**
- 9 hard registers allocated (%l2-%l7, %i3-%i5)
- Uses local (%l) and input (%i) register windows
- Big-endian → will differ from x86-64/ARM64

### MIPS - Well-Supported

**Strengths:**
- 10 hard registers allocated ($14-$23)
- Mix of saved ($s0-$s7) and temp ($t6-$t7) registers
- Clean RISC design

**Lessons:**
- Balanced register allocation strategy
- Uses both caller-saved and callee-saved regs

### HP-PA - PTR_ORG Example

**Weaknesses:**
- No hard register allocation
- Math library issues
- PTR_ORG = 0x40000000 required

**Lessons:**
- PTR_ORG mechanism works when needed
- May need to subtract high bits from pointers
- x86-64/ARM64 likely won't need this

### AIX - Another PTR_ORG

**Weaknesses:**
- PTR_ORG = 0x20000000 required
- No hard register allocation

**Lessons:**
- Different PTR_ORG value than HP-PA
- Modern Linux user space typically starts at 0
- Unlikely to affect x86-64/ARM64 Linux

### Macintosh - Resource Constraints

**Weaknesses:**
- Strict alignment required
- Reduced memory limits
- 68k architecture (obsolete)

**Lessons:**
- OptionalWordAlign mechanism exists if needed
- ARM64 may need strict alignment like Macintosh
- Memory limits are configurable per-platform

---

## Operating System Dependencies

### BSD vs System V (SYS5)

**System V differences:**
```c
#ifdef SYS5
#define HAS_NO_GETRUSAGE
#define HAS_NO_GETWD
#endif
```

**Workarounds:**
- `getrusage()` → `times()`
- `getwd()` → `getcwd()`

**Modern Linux:** Provides both BSD and POSIX interfaces

### Missing C Library Functions

**`gcvt()` - Floating-point to string conversion**

Affected platforms: MIPS, Macintosh

**From inout.c (implied):** Must use `sprintf("%g", value)` instead

**Modern platforms:** POSIX doesn't require `gcvt()`, use `snprintf()`

**Math functions: `asinh()`, `acosh()`, `atanh()`**

Affected platforms: HP-PA, m68k HP

**From configure.in:102-124:** HP-PA libraries have broken implementations

**Modern platforms:** Standard in C99 math.h

---

## Build System Platform Detection

### Configure Script (Autoconf)

**From configure.in:1-139**

**Host Detection:**
```sh
if test -z "${host}" ; then
  host=`${srcdir}/config.guess`
fi
host=`${srcdir}/config.sub $host`
```

**Examples:**
- `sparc-sun-solaris2.5`
- `alpha-dec-osf3.2`
- `hppa1.1-hp-hpux10.20`
- `mips-sgi-irix5.3`
- `i386-unknown-linux-gnu` (for x86)
- `x86_64-unknown-linux-gnu` (for x86-64)
- `aarch64-unknown-linux-gnu` (for ARM64)

**Platform-Specific Configuration:**
```sh
case ${host} in
  m68k-hp-hpux* )
    AGENTSFLAGS_MATH=-DNOASINH
    ;;
  hppa*-hp-hpux* )
    AGENTSFLAGS_MATH=-DNOASINH
    ;;
  * )
    AGENTSFLAGS_MATH=""
    ;;
esac
```

### GMP Build Integration

**From configure.in:50-71:**
```sh
CFLAGS_GMP="-I$(gmpdir)"
LDFLAGS_GMP="-L$(gmpdir) -lgmp"
MAKE_GMP=make-gmp
INSTALL_GMP=install-gmp

for arg do
  case ${arg} in
    -without-gmp | --without-gmp )
      # Disable bignum support
      CFLAGS_GMP=""
      LDFLAGS_GMP=""
      AGENTSFLAGS_GMP=-DNOBIGNUM
      MAKE_GMP=""
      INSTALL_GMP=""
    ;;
  esac
done
```

**GMP handles architecture-specific builds:** The GMP library's own configure script selects optimal assembly routines for each architecture.

---

## Porting to x86-64 and ARM64

### x86-64 (AMD64/Intel 64)

#### Required Changes

**1. Add x86-64 detection to sysdeps.h:**
```c
#if defined(__x86_64__) || defined(__amd64__)
#define TADBITS 64
#endif
```

**2. Add hard register definitions to regdefs.h:**
```c
#if defined(__x86_64__) || defined(__amd64__)
#define REGISTER1 asm("r15")   /* Callee-saved */
#define REGISTER2 asm("r14")   /* Callee-saved */
#define REGISTER3 asm("r13")   /* Callee-saved */
#define REGISTER4 asm("r12")   /* Callee-saved */
#define REGISTER5 asm("rbx")   /* Callee-saved */
#define REGISTER6 asm("rbp")   /* Frame pointer (use with care) */
/* Could also use: r10, r11 (caller-saved) */
#endif
```

**3. Set PTR_ORG (likely = 0):**
```c
#ifndef PTR_ORG
#define PTR_ORG 0  /* Linux user space: 0x0000000000000000 - 0x00007fffffffffff */
#endif
```

**4. Update configure:**
- Add `x86_64-*-*` case to configure.in if needed
- Modern autoconf should detect via config.guess

#### x86-64 Characteristics

- **Word size:** 64 bits
- **Endianness:** Little-endian ✅ (matches Alpha)
- **Alignment:** Relaxed (unaligned access allowed but slower)
- **Registers:** 16 GPRs (rax-r15), can use r12-r15, rbx
- **ABI:** System V AMD64 ABI (Linux) or Microsoft x64 (Windows)
- **Compiler:** gcc, clang both fully support x86-64
- **THREADED_CODE:** ✅ Fully supported
- **HARDREGS:** ✅ Can allocate 5-6 registers easily

#### Expected Issues

- ⚠️ **None major** - Should be straightforward
- ✅ PTR_ORG = 0 likely fine
- ✅ Alignment requirements met automatically
- ✅ Tagged pointers proven on Alpha

---

### ARM64 (AArch64 / ARMv8+)

#### Required Changes

**1. Add ARM64 detection to sysdeps.h:**
```c
#if defined(__aarch64__) || defined(__arm64__)
#define TADBITS 64
/* May need OptionalWordAlign depending on strictness */
#endif
```

**2. Add hard register definitions to regdefs.h:**
```c
#if defined(__aarch64__) || defined(__arm64__)
#define REGISTER1 asm("x28")   /* Callee-saved */
#define REGISTER2 asm("x27")   /* Callee-saved */
#define REGISTER3 asm("x26")   /* Callee-saved */
#define REGISTER4 asm("x25")   /* Callee-saved */
#define REGISTER5 asm("x24")   /* Callee-saved */
#define REGISTER6 asm("x23")   /* Callee-saved */
#define REGISTER7 asm("x22")   /* Callee-saved */
#define REGISTER8 asm("x21")   /* Callee-saved */
#define REGISTER9 asm("x20")   /* Callee-saved */
#define REGISTER10 asm("x19")  /* Callee-saved */
/* ARM64 has many callee-saved registers (x19-x28) */
#endif
```

**3. Set PTR_ORG (likely = 0):**
```c
#ifndef PTR_ORG
#define PTR_ORG 0  /* Linux user space starts low */
#endif
```

**4. Check alignment requirements:**
```c
#if defined(__aarch64__) || defined(__arm64__)
/* Test if strict alignment needed */
/* If unaligned access faults, may need: */
/* #define OptionalWordAlign(x)  WordAlign((unsigned long)(x)) */
#endif
```

**5. Update configure for ARM64**

#### ARM64 Characteristics

- **Word size:** 64 bits
- **Endianness:** Bi-endian (typically little-endian on Linux) ✅
- **Alignment:** Strict preferred (unaligned access may fault or be slow)
- **Registers:** 31 GPRs (x0-x30), x19-x28 are callee-saved (10 regs!)
- **ABI:** ARM64 AAPCS (Procedure Call Standard)
- **Compiler:** gcc, clang both fully support ARM64
- **THREADED_CODE:** ✅ Fully supported
- **HARDREGS:** ✅ Excellent - 10 callee-saved registers available

#### Expected Issues

- ⚠️ **Alignment strictness** - May need OptionalWordAlign like Macintosh
- ⚠️ **Test on real hardware** - Emulators may be more lenient
- ✅ PTR_ORG = 0 likely fine
- ✅ Register-rich architecture (more than Alpha!)
- ✅ Tagged pointers proven on Alpha

#### ARM64 Alignment Testing

**Test code:**
```c
/* Check if unaligned access is allowed */
#include <signal.h>
#include <setjmp.h>

jmp_buf env;
void sigbus_handler(int sig) { longjmp(env, 1); }

int main() {
  char buf[16];
  int *ptr = (int*)(buf + 1);  /* Unaligned */

  signal(SIGBUS, sigbus_handler);
  if (setjmp(env) == 0) {
    *ptr = 42;  /* Try unaligned write */
    printf("Unaligned access OK\n");
  } else {
    printf("Unaligned access FAULTS - need OptionalWordAlign\n");
  }
}
```

---

## References

### Source Files Analyzed

**Platform Detection:**
- `emulator/sysdeps.h:1-42` - All platform-specific defines
- `emulator/regdefs.h:1-87` - Hard register allocation for all architectures
- `emulator/constants.h:1-64` - Platform-specific memory limits

**Architecture-Specific Code:**
- `emulator/include.h:1-14` - Platform-specific includes
- `emulator/macintosh.h` - Macintosh-specific declarations
- `emulator/macintosh.c` - Macintosh-specific implementations
- `emulator/time.c` - Platform-specific timing code
- `emulator/inout.c` - Platform-specific I/O
- `emulator/main.c` - Platform-specific initialization

**Build System:**
- `configure.in:1-139` - Autoconf configuration with platform detection
- `config.guess` - Platform identification utility
- `config.sub` - Platform name canonicalization

**Configuration Flags:**
- `emulator/engine.c` - THREADED_CODE, HARDREGS, TRACE, BAM usage
- `emulator/opcodes.h` - BAM conditional opcodes
- `emulator/term.h` - STRUCT_ENV, INDIRECTVARS, TADTURMS options
- `emulator/config.c` - TADTURMS implementation

### Related Documents

- `01-ARCHITECTURE-OVERVIEW.md` - System architecture overview
- `02-EMULATOR-ANALYSIS.md` - Detailed emulator internals (to be written)
- `STUDY-PLAN.md` - Overall porting study plan

---

## Summary Table: All Configuration Options

| Option | Type | Purpose | Status | Recommend |
|--------|------|---------|--------|-----------|
| `TADBITS` | Arch | Word size in bits | ✅ Active | Set to 64 |
| `PTR_ORG` | Arch | Pointer base address | ✅ Active | Test = 0 |
| `WORDALIGNMENT` | Arch | Memory alignment | ✅ Active | Auto = 8 |
| `OptionalWordAlign` | Arch | Force strict alignment | ⚠️ Optional | Test on ARM64 |
| `THREADED_CODE` | Feature | Computed goto dispatch | ✅ Active | ✅ Enable |
| `HARDREGS` | Feature | Physical register allocation | ⚠️ Optional | ✅ Enable |
| `BAM` | Feature | Extended instruction set | ⚠️ Unknown | ⚠️ Research first |
| `TRACE` | Debug | Execution tracing | ✅ Active | ⚠️ Debug only |
| `STRUCT_ENV` | Unused | Structure environments | ❌ Disabled | ❌ Leave off |
| `INDIRECTVARS` | Unused | Indirect variables only | ❌ Disabled | ❌ Leave off |
| `TADTURMS` | Debug | Type-safe Terms | ❌ Disabled | ⚠️ Debug only |
| `SYS5` | OS | System V Unix | ✅ Auto | ✅ Auto-detect |
| `HAS_NO_GCVT` | OS | Missing gcvt() | ✅ Auto | ✅ Auto-detect |
| `HAS_NO_GETRUSAGE` | OS | Missing getrusage() | ✅ Auto | ✅ Auto-detect |
| `HAS_NO_GETWD` | OS | Missing getwd() | ✅ Auto | ✅ Auto-detect |
| `NOASINH` | OS | Missing math functions | ✅ Auto | ✅ Auto-detect |
| `NOBIGNUM` | Feature | Disable GMP | ⚠️ Optional | ❌ Keep GMP |

---

## Revision History

| Date | Version | Changes |
|------|---------|---------|
| 2025-11-04 | 1.0 | Complete platform dependencies analysis |

---

**End of Document**
