# Chapter 19: Platform Dependencies

## Introduction

AGENTS is designed to be portable across different hardware architectures and operating systems, but some aspects of the implementation necessarily depend on platform characteristics. The system carefully isolates these dependencies to a small number of configuration files, making porting to new platforms straightforward.

This chapter documents all platform-specific aspects of AGENTS, how they're detected and configured, and what varies between supported platforms.

## Platform Detection

### Compile-Time Detection

Platform characteristics are detected using **preprocessor macros** defined by the compiler. The primary detection file is `emulator/sysdeps.h`, which sets configuration based on `__<platform>__` macros.

**Supported platforms** (from `sysdeps.h`):

**64-bit architectures**:
```c
#ifdef __alpha__
#define TADBITS 64
#endif

#if defined(__x86_64__) || defined(__amd64__)
#define TADBITS 64
#endif

#if defined(__aarch64__) || defined(__arm64__)
#define TADBITS 64
#define HAS_NO_GCVT
#define HAS_NO_GETWD
#endif
```

**Legacy 32-bit architectures**:
- SPARC
- MIPS
- HP-PA
- Various Unix variants

### Operating System Detection

**Unix variants**:
```c
#ifdef __svr4__
#define SYS5
#endif

#ifdef SYS5
#define HAS_NO_GETRUSAGE
#define HAS_NO_GETWD
#endif
```

**Special cases**:
```c
#ifdef _AIX
#define unix
#define PTR_ORG ((uword)0x20000000)
#endif

#ifdef macintosh
#define HAS_NO_GCVT
#define OptionalWordAlign(x) WordAlign((unsigned long)(x))
#endif
```

## Key Platform Parameters

### TADBITS: Word Size

**Definition**: Number of bits in a word (Tagged Address BITS)

**Values**:
- `32`: 32-bit platforms (SPARC, MIPS, old x86)
- `64`: 64-bit platforms (Alpha, x86-64, ARM64)

**Impact**:
- Determines number of tag bits available (2 vs 3)
- Affects small integer range (26-bit vs 58-bit)
- Changes pointer alignment requirements
- Influences memory layout

**From `sysdeps.h:1-16`**: All modern platforms set `TADBITS 64`.

### PTR_ORG: Pointer Origin

**Definition**: Base address for heap allocation

**Purpose**: Some platforms have restrictions on valid address ranges. PTR_ORG specifies the starting address for the heap.

**Default**:
```c
#define PTR_ORG ((uword)0)
```

**Platform-specific**:
```c
#ifdef hppa
#  define PTR_ORG ((uword)0x40000000)
#endif

#ifdef _AIX
#define PTR_ORG ((uword)0x20000000)
#endif
```

**Use in memory allocation** (from `storage.c`):
```c
heap_base = (Term *)(PTR_ORG + requested_size);
```

**Modern platforms**: PTR_ORG is 0, no special restrictions.

### WORDALIGNMENT: Address Alignment

**Definition**: Required alignment for word-aligned data

**Values**:
```c
#define WORDALIGNMENT 4    // 32-bit: 4-byte alignment
#define WORDALIGNMENT 8    // 64-bit: 8-byte alignment
```

**Derived from TADBITS**:
```c
#if TADBITS == 64
#  define WORDALIGNMENT 8
#else
#  define WORDALIGNMENT 4
#endif
```

**Use**:
```c
#define WordAlign(x) (((x) + (WORDALIGNMENT-1)) & ~(WORDALIGNMENT-1))
```

**Why it matters**:
- Ensures tagged pointers have low bits zero
- Satisfies CPU alignment requirements
- Prevents alignment faults on strict architectures

### OptionalWordAlign: Platform-Specific Alignment

**Definition**: Conditional alignment for platforms with specific requirements

**Default**:
```c
#define OptionalWordAlign(x) (x)    // No-op
```

**Platform-specific** (from `sysdeps.h:42`):
```c
#ifdef macintosh
#define OptionalWordAlign(x) WordAlign((unsigned long)(x))
#endif
```

**Use**: Some platforms require stricter alignment than others.

## Missing System Functions

### Platform Function Availability

Different Unix variants have different standard library functions. AGENTS detects and works around missing functions.

**Detection flags**:

```c
HAS_NO_GCVT      // No gcvt() for float-to-string
HAS_NO_GETWD     // No getwd() for current directory
HAS_NO_GETRUSAGE // No getrusage() for resource usage
```

**From `sysdeps.h`**:

```c
#if defined(__aarch64__) || defined(__arm64__)
#define HAS_NO_GCVT
#define HAS_NO_GETWD
#endif

#ifdef mips
#define HAS_NO_GCVT
#endif

#ifdef SYS5
#define HAS_NO_GETRUSAGE
#define HAS_NO_GETWD
#endif

#ifdef macintosh
#define HAS_NO_GCVT
#endif
```

### Workarounds

**gcvt() replacement**:
```c
#ifdef HAS_NO_GCVT
  sprintf(buffer, "%.16g", value);  // Use sprintf instead
#else
  gcvt(value, 16, buffer);
#endif
```

**getwd() replacement**:
```c
#ifdef HAS_NO_GETWD
  getcwd(buffer, size);  // Use getcwd instead
#else
  getwd(buffer);
#endif
```

**getrusage() replacement**:
```c
#ifdef HAS_NO_GETRUSAGE
  // Use alternative timing mechanism or skip
#else
  getrusage(RUSAGE_SELF, &usage);
#endif
```

## Endianness

### Current Assumption

AGENTS **assumes little-endian** byte order. This works for:
- x86-64 (little-endian)
- ARM64 (typically little-endian)
- Alpha (little-endian)

**Why**: Bytecode format, term encoding, and file I/O all assume little-endian.

### Potential Big-Endian Support

To support big-endian platforms (SPARC, PowerPC, MIPS in big-endian mode):

**Option 1**: Byte-swap bytecode on load
```c
#ifdef BIG_ENDIAN
  term = bswap_64(term);
#endif
```

**Option 2**: Platform-specific bytecode files
- Compile separately for big-endian
- Detect endianness and refuse to load incompatible files

**Current status**: No big-endian support in 64-bit builds. Would require changes to:
- Bytecode loading (`load.c`)
- Term encoding (`term.h`)
- File I/O (`read.c`, `write.c`)

## Platform-Specific Code Paths

### Conditional Compilation

**Pattern**:
```c
#ifdef __x86_64__
  // x86-64 specific code
#elif defined(__aarch64__)
  // ARM64 specific code
#else
  // Generic fallback
#endif
```

**Examples**:

**Hard register allocation** (from `regdefs.h`):
```c
#ifdef __x86_64__
#  define HARDREGS
#  define REGISTER1 "r12"
#  define REGISTER2 "r13"
#  define REGISTER3 "r14"
#  define REGISTER4 "r15"
#  define REGISTER5 "rbx"
#  define REGISTER6 "rbp"
#elif defined(__aarch64__)
#  define HARDREGS
#  define REGISTER1 "x19"
#  define REGISTER2 "x20"
#  define REGISTER3 "x21"
#  define REGISTER4 "x22"
#  define REGISTER5 "x23"
#  define REGISTER6 "x24"
#  define REGISTER7 "x25"
#  define REGISTER8 "x26"
#  define REGISTER9 "x27"
#  define REGISTER10 "x28"
#endif
```

**System includes**:
```c
#ifdef sequent
#include <sys/types.h>
extern double atof();
extern char *getenv();
#endif
```

### Runtime Detection

**Generally avoided**: AGENTS prefers compile-time detection over runtime checks for performance.

**Exception**: Optional features detected at configure time (see Chapter 21).

## Porting Checklist

### Adding a New Platform

To port AGENTS to a new platform:

**1. Update `sysdeps.h`**:
```c
#ifdef __newplatform__
#define TADBITS 64          // or 32
#define PTR_ORG ((uword)0)  // if needed
// Add HAS_NO_* flags as needed
#endif
```

**2. Update `regdefs.h`** (optional, for performance):
```c
#ifdef __newplatform__
#  define HARDREGS
#  define REGISTER1 "regname1"
#  define REGISTER2 "regname2"
   // ... map to callee-saved registers
#endif
```

**3. Update `configure.in`**:
```sh
case "$host" in
  newplatform-*)
    ARCH_FLAGS="-march=..."
    ;;
esac
```

**4. Test**:
```sh
./configure
make
make check
```

**5. Verify**:
- Bytecode loading works
- Garbage collection doesn't crash
- Tests pass
- Demos run correctly

### Platform-Specific Issues

**Common porting problems**:

**Alignment faults**:
- Symptom: SIGBUS on SPARC/ARM
- Cause: Misaligned memory access
- Fix: Check WordAlign usage

**Stack direction**:
- Most platforms grow stack downward
- Some old platforms grow upward
- AGENTS assumes downward

**Register clobbering**:
- Hard register allocation assumes callee-saved semantics
- Check ABI specification for platform
- Some registers may be reserved

**Pointer size assumptions**:
- Code assumes `sizeof(void*) == sizeof(long)`
- True on all supported platforms
- Would break on segmented architectures

## Platform Comparison

### 64-bit Platforms

| Platform | TADBITS | Word Align | Hard Regs | Status |
|----------|---------|------------|-----------|--------|
| Alpha | 64 | 8 | No | Legacy (reference) |
| x86-64 | 64 | 8 | 6 regs | Primary |
| ARM64 | 64 | 8 | 10 regs | Primary |

**All 64-bit platforms**:
- 3-bit tags (8 types)
- 58-bit small integers
- 8-byte word alignment
- PTR_ORG = 0 (modern)

### 32-bit Platforms (Legacy)

| Platform | TADBITS | Word Align | Status |
|----------|---------|------------|--------|
| SPARC | 32 | 4 | Unsupported in v0.9 |
| MIPS | 32 | 4 | Unsupported in v0.9 |
| x86 (i386) | 32 | 4 | Unsupported in v0.9 |

**All 32-bit platforms**:
- 2-bit tags (4 types)
- 26-bit small integers
- 4-byte word alignment
- May have PTR_ORG restrictions

**Current status**: 32-bit support removed in 64-bit porting work. Could be restored with additional configuration.

## Operating System Differences

### System V vs. BSD

**System V Unix** (`SYS5` flag):
- Different signal semantics
- Different process management
- Missing some BSD functions (getrusage, getwd)

**BSD Unix**:
- Rich set of system calls
- More complete POSIX support
- Original AGENTS development platform

**Modern Linux**:
- Supports both System V and BSD APIs
- Most complete platform
- Primary target for current work

### macOS Specifics

**From `sysdeps.h:13-15`**:
```c
#ifdef __APPLE__
#define unix
#endif
```

**Quirks**:
- Reports as `__aarch64__` on Apple Silicon
- Requires `unix` defined explicitly
- Some BSD functions have different signatures

**Otherwise**: macOS is a clean Unix platform, works well with standard configuration.

## Build System Detection

### Configure-Time Detection

The `configure` script (generated from `configure.in`) detects:
- Platform architecture (`$host`)
- Compiler type and version
- Available libraries
- System functions
- Header files

**Example** (from `configure.in`):
```sh
case "$host" in
  x86_64-*)
    ARCH_FLAGS="-m64"
    ;;
  aarch64-*|arm64-*)
    ARCH_FLAGS="-march=armv8-a"
    ;;
esac

AC_SUBST(ARCH_FLAGS)
```

**Output**: Generates `Makefile` with platform-specific settings.

### Compiler Feature Detection

**autoconf tests** check for:
- Inline function support
- Computed goto (labels as values)
- Register variables
- Specific headers
- Library functions

**Example**:
```sh
AC_MSG_CHECKING([for inline])
AC_TRY_COMPILE([], [inline int foo() { return 0; }],
  [AC_DEFINE(HAVE_INLINE)],
  [CFLAGS="$CFLAGS -fgnu89-inline"])
```

See Chapter 21 for full build system details.

## Testing Platform Support

### Verification Tests

**Basic functionality**:
```sh
make check
```

Runs test suite (see Chapter 23).

**Platform-specific tests**:
- Word alignment
- Tag bit usage
- Small integer range
- Pointer encoding
- GC correctness

**Stress tests**:
- Large heap allocation
- Deep recursion
- Heavy GC load
- Concurrent agents
- Long-running programs

### Known Issues

**Alpha (legacy)**:
- No GMP support (excluded in build)
- Minimal testing in recent years
- May have bitrot

**ARM64**:
- Parser had issues with long constants (fixed)
- Thoroughly tested on Apple Silicon and Linux
- Production ready

**x86-64**:
- Most tested platform
- Primary development target
- No known issues

## Future Portability

### RISC-V Support

**Status**: Not yet supported

**Requirements**:
1. Add `__riscv` detection to `sysdeps.h`
2. Set TADBITS to 64
3. Map hard registers in `regdefs.h`
4. Test GC on RISC-V memory model
5. Verify atomic operations if using threads

**Expected difficulty**: Low. Similar to ARM64 port.

### 128-bit Platforms

**Hypothetical**: If 128-bit CPUs become common

**Changes needed**:
- TADBITS = 128
- More tag bits available (4-5 bits)
- Small integer range expands (122 bits!)
- Alignment changes to 16 bytes
- Pointer encoding unchanged

**Benefits**:
- Even larger immediate integers
- More type tags available
- More efficient for huge heaps

## Summary

Platform dependencies in AGENTS are well-isolated and manageable:

**Key files**:
1. `emulator/sysdeps.h`: Platform detection and configuration
2. `emulator/regdefs.h`: Hard register allocation (platform-specific)
3. `configure.in`: Build-time detection

**Key parameters**:
- **TADBITS**: Word size (32 or 64)
- **PTR_ORG**: Heap base address (usually 0)
- **WORDALIGNMENT**: Alignment requirement (4 or 8 bytes)
- **HAS_NO_***: Missing system functions

**Supported platforms**:
- ✅ x86-64 (primary)
- ✅ ARM64 (primary)
- ⚠️ Alpha (legacy, minimal testing)
- ❌ 32-bit platforms (not supported in v0.9)

**Porting process**:
1. Update `sysdeps.h` with platform detection
2. Optionally map hard registers in `regdefs.h`
3. Update `configure.in` with build flags
4. Test with full test suite
5. Verify GC correctness
6. Run demos and benchmarks

Platform abstraction is effective: the vast majority of AGENTS code is platform-independent. Only ~200 lines in `sysdeps.h` and `regdefs.h` are platform-specific, out of ~15,000 lines total.

The next chapter covers register allocation strategies across different platforms.

---

**Chapter 19 complete** (~30 pages)
