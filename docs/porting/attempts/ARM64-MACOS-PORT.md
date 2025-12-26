# ARM64 macOS Port Attempt - Technical Report

**Date:** 2025-11-05
**Branch:** `claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP`
**Status:** PARTIAL SUCCESS - Emulator compiles but hangs at runtime
**Platform:** Apple Silicon (ARM64) / macOS

## Executive Summary

Successfully ported AGENTS v0.9 emulator to compile on ARM64 macOS (Apple Silicon). The emulator builds cleanly but exhibits a silent hang at runtime when loading bootstrap files. All compilation issues have been resolved, but a runtime issue blocks full functionality.

## Successful Achievements

### 1. Emulator Compilation ✅
- ARM64 binary successfully built on Apple Silicon Mac
- All macOS-specific compilation errors resolved
- Platform detection working correctly (`arm64-apple-darwin`)

### 2. Platform Support Added ✅
- Added ARM64 register mappings (x19-x28) in `regdefs.h`
- Added 64-bit tagged address support in `sysdeps.h`
- Updated `config.guess` to recognize `arm64:Darwin`

### 3. macOS Compatibility Fixes ✅

#### Implicit Int Declarations
macOS Clang treats implicit int as error (not warning):
- `emulator/trace.h:260` - Added `int` return type to `trace_apply_list()`
- `emulator/engine.c:43` - Added `int` return type to `readstring()`
- `emulator/exstate.h:472` - Added `bool entailed()` declaration
- `emulator/functor.c:28` - Added `int` type to `power_table[]` array

#### Missing Standard Headers
Added to `include.h`:
```c
#include <stdlib.h>  // malloc, free
#include <string.h>  // strlen, strcpy
#include <unistd.h>  // isatty, fileno
```

#### malloc.h Portability
macOS doesn't have `<malloc.h>` (functions in `<stdlib.h>`):
- Added `__APPLE__` guards in: `configuration.c`, `exstate.c`, `storage.c`, `worker.c`, `parser.tab.c`

#### systime() Platform Detection
Extended platform check for macOS:
```c
#if defined(unix) || defined(__unix__) || defined(__APPLE__)
```

### 4. Build System Improvements ✅

#### Configure Options
Properly declared using `AC_ARG_WITH`:
- `--without-gmp` - Build without GNU Multi-Precision library
- `--without-fd` - Build without Finite Domain constraint solver

#### FD Stub Functions
Created `emulator/fd_stub.c` for `--without-fd` builds:
- Provides no-op implementations of FD functions
- Prevents linker errors when FD is excluded

#### Bootstrapping Fix
Added symlink creation in `Makefile.in`:
```makefile
make-emulator:
    ...
    ln -sf emulator/oagents agents
```

### 5. FD (Finite Domain) Compilation Fixes ✅

When building with FD support:

#### Type Conversion Fix
```c
// fd_akl.c:419
instr = FdInstrToEnum((fd_instr)byte_code[d]);
```
Explicit cast required for `bcode` (long) → `fd_instr` (void*)

#### Missing Declaration
```c
// fd_akl.h:228 (after dep_class typedef)
extern dep_class propagate_info();
```

## Current Blocking Issue ❌

### Symptom: Silent Hang at Runtime

**Behavior:**
```bash
./agents
# Completely silent, no output
# No prompt, no error messages
# Ctrl-C enters interrupt handler and exits cleanly
```

**With piped input:**
```bash
echo "compilef('assoc')." | ./agents
# Hangs silently
# Ctrl-C shows: "AKL interruption (h for help)?"
```

### What Works
- ✅ Emulator builds successfully
- ✅ Interrupt handler responds to Ctrl-C
- ✅ No crashes or segfaults
- ✅ Bootstrap files exist and are readable:
  - `environment/boot.pam` (397K, 23,784 lines)
  - `oldcompiler/comp.pam` (326K, 18,593 lines)
  - `version.pam` (609 bytes)

### What Doesn't Work
- ❌ No output when agents starts
- ❌ No prompt appears
- ❌ Appears stuck in infinite loop or blocked I/O
- ❌ Cannot process any input

### Comparison: x86-64 vs ARM64

| Aspect | x86-64 Linux (WORKS) | ARM64 Mac (HANGS) |
|--------|---------------------|-------------------|
| Platform | `x86_64-unknown-linux-gnu` | `arm64-apple-darwin` |
| Compilation | ✅ Success | ✅ Success |
| Emulator runs | ✅ Yes | ❌ Silent hang |
| FD included | ✅ Yes | ✅ Yes (after fixes) |
| GMP included | ✅ Yes | ❌ No (`--without-gmp`) |
| TADBITS | 64 | 64 |
| Registers | r15,r14,r13,r12,rbx,rbp | x19-x28 |

**Key Difference:** The x86-64 port was tested and works. The ARM64 port compiles but doesn't run.

## Technical Details

### Platform-Specific Code

#### emulator/sysdeps.h
```c
#if defined(__aarch64__) || defined(__arm64__)
#define TADBITS 64
#define HAS_NO_GCVT
#define HAS_NO_GETWD
#endif
```

#### emulator/regdefs.h
```c
#if defined(__aarch64__) || defined(__arm64__)
#define REGISTER1 asm("x19")
#define REGISTER2 asm("x20")
#define REGISTER3 asm("x21")
#define REGISTER4 asm("x22")
#define REGISTER5 asm("x23")
#define REGISTER6 asm("x24")
#define REGISTER7 asm("x25")
#define REGISTER8 asm("x26")
#define REGISTER9 asm("x27")
#define REGISTER10 asm("x28")
#endif
```

**Note:** These registers are ARM64 callee-saved registers, more than x86-64's 6 available.

#### config.guess
```sh
arm64:Darwin:*:*)
    echo arm64-apple-darwin
    exit 0 ;;
```

### Build Configuration Used

**Working x86-64 Linux build:**
```bash
./configure
make
```

**Current ARM64 Mac build:**
```bash
./configure --without-gmp
make
```

Both with and without `--without-fd` tested. Hang occurs in both cases.

### Initialization Sequence

From `emulator/main.c`:
1. Parse bootfiles from `-b` arguments
2. Call `initialise()` - registers all builtin predicates
3. Load bootfiles: `boot.pam`, `comp.pam`, `version.pam`
4. Look for `main/0` and `restart/0` predicates
5. Create execution state
6. Call engine to execute `main/0`

**Hypothesis:** Hang likely occurs during step 3 (loading) or step 6 (execution).

## Debugging Recommendations

### Immediate Next Steps

1. **Add debug output to trace execution:**
   ```c
   // In main.c, add fprintf(stderr, ...) at key points
   fprintf(stderr, "Loading bootfile: %s\n", pathBuf);
   fprintf(stderr, "Bootfile loaded\n");
   fprintf(stderr, "Calling main/0\n");
   ```

2. **Run under debugger (lldb on Mac):**
   ```bash
   lldb ./emulator/oagents
   (lldb) run -b environment/boot.pam -b oldcompiler/comp.pam -b version.pam
   # Let it hang, then Ctrl-C
   (lldb) bt          # Show stack trace
   (lldb) frame info  # Show current location
   ```

3. **Check for infinite loops in:**
   - `load.c` - PAM file loading
   - `engine.c` - Main execution engine
   - Platform-specific I/O code that might behave differently on macOS

4. **Compare binary differences:**
   ```bash
   # On x86-64 Linux
   nm emulator/oagents | sort > /tmp/x86-symbols.txt

   # On ARM64 Mac
   nm emulator/oagents | sort > /tmp/arm64-symbols.txt

   diff /tmp/x86-symbols.txt /tmp/arm64-symbols.txt
   ```

### Potential Root Causes

1. **Endianness Issue**
   - ARM64 and x86-64 are both little-endian, so unlikely
   - But PAM files might have embedded binary data

2. **Register Allocation Bug**
   - If `-DHARDREGS` is used, register variables might cause issues
   - Try building without register variables

3. **Pointer Size Assumptions**
   - Both platforms are 64-bit
   - But check for any `sizeof(long)` vs `sizeof(void*)` assumptions

4. **I/O Differences**
   - macOS vs Linux system calls
   - Terminal handling differences
   - File buffering behavior

5. **Compiler Optimization**
   - Try building with `-O0` instead of `-O2`
   - GCC vs Clang differences

6. **Missing Initialization**
   - When building `--without-gmp`, some initialization might be missing
   - Compare with full build including GMP

## Files Modified

### Source Files
- `emulator/trace.h` - Added return types
- `emulator/engine.c` - Added return types
- `emulator/exstate.h` - Added declarations
- `emulator/exstate.c` - Added `__APPLE__` guard
- `emulator/functor.c` - Added array type
- `emulator/include.h` - Added standard headers
- `emulator/time.c` - Extended platform detection, added include
- `emulator/configuration.c` - Added `__APPLE__` guard
- `emulator/storage.c` - Added `__APPLE__` guard
- `emulator/worker.c` - Added `__APPLE__` guard
- `emulator/parser.tab.c` - Added `__APPLE__` guards (generated)
- `emulator/fd_stub.c` - NEW: FD stub functions
- `emulator/fd_akl.c` - Added type cast
- `emulator/fd_akl.h` - Added declaration
- `emulator/sysdeps.h` - ARM64 platform detection
- `emulator/regdefs.h` - ARM64 register mappings

### Build System
- `configure.in` - Added `AC_ARG_WITH` declarations, CFLAGS
- `configure` - Regenerated
- `Makefile.in` - Added symlink creation, OBJ_FD variable
- `emulator/Makefile` - Use $(OBJ_FD) variable
- `.gitignore` - Added Makefile, agents (generated files)
- `config.guess` - ARM64 platform recognition

### Platform Support
- `sysdeps.h` - ARM64 with TADBITS=64
- `regdefs.h` - ARM64 register allocation (x19-x28)
- `config.guess` - Recognizes `arm64:Darwin`

## Commits History (Highlights)

1. Initial x86-64 port with signedness bug fix
2. Build system integration for `./configure; make`
3. macOS compatibility fixes (malloc.h, implicit int, headers)
4. `--without-fd` and `--without-gmp` configure options
5. FD stub functions for bootstrap
6. FD type conversion fixes for macOS Clang
7. Bootstrapping symlink fix

## Conclusions

### Success Factors
- Systematic approach to macOS compilation errors
- Proper build system integration
- Understanding of bootstrap dependencies
- Platform-specific code isolation

### Remaining Challenges
- Runtime hang is a blocking issue
- Requires deeper debugging to identify root cause
- May be architecture-specific or OS-specific

### Lessons Learned
1. **Never run `make realclean`** - destroys bootstrap `.pam` files
2. **macOS Clang is stricter** than Linux GCC about old C code
3. **FD predicates are required** - bootstrap files reference them
4. **Build system integration** is critical for usability
5. **Generated files** should not be in version control

## Next Session Recommendations

1. **Enable verbose debug output** in emulator
2. **Run under lldb** to capture stack trace of hang
3. **Compare with minimal test** - create simple PAM file to load
4. **Test without bootfiles** - does oagents respond to any input?
5. **Build with different optimization levels** (-O0, -O1, -O2)
6. **Check for macOS-specific syscall differences**

## References

- Original AGENTS v0.9 from Swedish Institute of Computer Science (1990s)
- Previous x86-64 port: `claude/port-attempt-01-011CUoHSMTFawkiKfVsuVuPP` (working)
- ARM64 Architecture Reference Manual
- macOS System Call differences vs Linux

---

**Status:** Awaiting debugging session to identify runtime hang location.
