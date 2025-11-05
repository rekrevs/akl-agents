# ARM64 Minimal Port Strategy

**Branch:** `claude/arm64-minimal-port-011CUoHSMTFawkiKfVsuVuPP`
**Base:** `5b3db2c` - First working x86-64 port (FULLY TESTED AND WORKING)

## Philosophy

**Start from success, not from experiments.**

The previous ARM64 attempt (`claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP`) made 20+ changes trying to fix compilation issues and adding features like `--without-fd`. It compiled but **hung at runtime**.

This time: **Minimal, surgical changes only.**

## Starting Point Status

✅ **Base commit works perfectly on x86-64 Linux:**
```bash
$ ./agents -b boot.pam -b comp.pam -b version.pam
AGENTS 0.9 (threaded code): Wed Aug 14 14:46:52 MET DST 2002
| ?- X is 10 * 10.
X = 100 ?
```

**What we have:**
- MaxSmallNum signedness fix (critical!)
- x86-64 platform support
- Working interactive system
- Proven runtime behavior
- All bootstrap files intact

**What we DON'T have:**
- ARM64 platform support
- macOS compatibility fixes
- Build system improvements

## Minimal Changes Required

### Phase 1: Platform Detection (2 files)

**1. `emulator/sysdeps.h` - Add ARM64 TADBITS**
```c
#if defined(__aarch64__) || defined(__arm64__)
#define TADBITS 64
#endif
```

**2. `emulator/regdefs.h` - Add ARM64 registers**
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

**3. `config.guess` - Recognize arm64-apple-darwin**

### Phase 2: Essential macOS Compatibility (if needed)

**Only add macOS fixes IF compilation fails:**
- malloc.h → stdlib.h (if needed)
- Missing function declarations (if needed)
- System call differences (if needed)

**Rule:** Don't pre-fix things. Fix only what breaks.

### Phase 3: Test on macOS

Build and test:
```bash
./configure
make
./agents -b environment/boot.pam -b oldcompiler/comp.pam -b version.pam
```

**Expected result:** It should either:
1. ✅ Work immediately (best case)
2. ❌ Show specific compilation error (fix minimally)
3. ❌ Show runtime error (debug carefully)

## What We're AVOIDING

**No premature optimizations:**
- ❌ No `--without-fd` (x86-64 has FD and works)
- ❌ No `--without-gmp` (x86-64 has GMP and works)
- ❌ No build system changes (x86-64 build works as-is)
- ❌ No extra header includes (unless compilation fails)
- ❌ No FD stubs (keep FD code, it might be fine)
- ❌ No reorganization of code structure

**Principle:** If x86-64 doesn't need it, ARM64 probably doesn't either.

## Comparison Strategy

### Working x86-64 Baseline
- Compilation flags: Default from configure
- Features: FD enabled, GMP enabled
- Platform: `x86_64-unknown-linux-gnu`
- Result: **Boots and runs interactively**

### ARM64 Target
- Compilation flags: **Same as x86-64**
- Features: **Same as x86-64** (FD enabled, GMP enabled)
- Platform: `arm64-apple-darwin`
- Result: **Should behave identically**

## Risk Mitigation

**If it hangs like before:**
1. We have a minimal diff to analyze (< 50 lines vs 300+ lines)
2. We can compare directly with working x86-64
3. We can bisect changes more easily
4. We know x86-64 behavior as reference

**If compilation fails:**
1. Add only the specific fix needed
2. Document why it's necessary
3. Keep changes minimal and surgical
4. Test after each change

## Success Criteria

**Stage 1: Compilation**
- ✅ ARM64 emulator compiles cleanly
- ✅ No warnings about architecture
- ✅ All object files created

**Stage 2: Linking**
- ✅ Binary links successfully
- ✅ No undefined symbols
- ✅ GMP links (same as x86-64)

**Stage 3: Runtime**
- ✅ Boots without hanging
- ✅ Loads bootstrap files
- ✅ Shows prompt
- ✅ Accepts input

**Stage 4: Functionality**
- ✅ Executes queries
- ✅ Performs unification
- ✅ Arithmetic works
- ✅ Interactive loop functions

## Debugging Plan

**If it hangs:**
1. Compare with x86-64 at binary level
2. Add strategic printf() to trace execution
3. Use lldb to capture hang location
4. Check for endianness issues
5. Check for alignment issues
6. Verify register usage (HARDREGS flag)

**Key difference from last time:** We'll know exactly where x86-64 succeeds and where ARM64 diverges.

## Documentation

All changes will be documented in:
- `docs/attempts/ARM64-MINIMAL-PORT.md` (this file)
- Commit messages with rationale
- Comparison notes with x86-64

## Timeline

1. **Now:** Add ARM64 platform detection (3 files, ~30 lines)
2. **Test:** Try to build on macOS
3. **Fix:** Address only what breaks
4. **Test:** Run on macOS
5. **Debug:** If hangs, compare with x86-64 systematically

---

**Strategy:** Copy success, not experiments. Minimal changes. Test often.
