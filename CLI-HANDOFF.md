# CLI Session Handoff - ARM64 Minimal Port

## Current Status

**Branch:** `claude/arm64-minimal-port-011CUoHSMTFawkiKfVsuVuPP`
**Base Commit:** `5b3db2c` - First fully working x86-64 port

## What's Ready

✅ Fresh branch created from **proven working** x86-64 commit
✅ Strategy documented in `docs/attempts/ARM64-MINIMAL-PORT.md`
✅ Branch pushed to remote

## What You Need to Do

### 1. Add ARM64 Platform Support (3 files, ~30 lines total)

**File 1: `emulator/sysdeps.h`**
Add after the x86-64 block:
```c
#if defined(__aarch64__) || defined(__arm64__)
#define TADBITS 64
#define HAS_NO_GCVT
#define HAS_NO_GETWD
#endif
```

**File 2: `emulator/regdefs.h`**
Add after the x86-64 block (line 56):
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

**File 3: `config.guess`**
Add ARM64 macOS detection (around line 177-185):
```sh
aarch64:Linux:*:*)
    echo aarch64-unknown-linux-gnu
    exit 0 ;;
arm64:Darwin:*:*)
    echo arm64-apple-darwin
    exit 0 ;;
```

### 2. Commit and Test

```bash
git add emulator/sysdeps.h emulator/regdefs.h config.guess
git commit -m "ADD: ARM64 platform support (minimal)"
git push
```

### 3. Try to Build

```bash
./configure
make clean
make
```

### 4. If It Compiles Successfully

Test it:
```bash
./agents -b environment/boot.pam -b oldcompiler/comp.pam -b version.pam
```

**If it works:** 🎉 Success! ARM64 port complete with minimal changes!

**If it hangs:** We have a minimal diff to debug (unlike the 300+ line previous attempt)

### 5. If Compilation Fails

**Only fix what breaks!** Add the specific fix needed, commit it, document why.

Common macOS issues (add ONLY if compilation fails):
- malloc.h not found → Add `#ifndef __APPLE__` guard
- Implicit int declarations → Add return type
- Missing stdlib.h → Add `#include <stdlib.h>` to include.h

## Key Principles

1. **Minimal changes** - If x86-64 doesn't have it, don't add it
2. **Test often** - Build after each change
3. **Document why** - Each fix should have a reason
4. **Compare with x86-64** - It's our working baseline

## Previous Attempt Lessons

The previous ARM64 attempt (`claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP`):
- Made 20+ changes
- Added `--without-fd` and `--without-gmp`
- Compiled successfully
- **Hung silently at runtime**

This attempt:
- Start from working x86-64 (not experiments)
- Keep FD and GMP (like x86-64)
- Add only platform detection
- If it hangs, we have minimal diff to analyze

## Reference

Working x86-64 behavior:
```bash
$ ./agents -b boot.pam -b comp.pam -b version.pam
AGENTS 0.9 (threaded code): Wed Aug 14 14:46:52 MET DST 2002
(C) 1990-1994 Swedish Institute of Computer Science
All rights reserved.
| ?- X is 10 * 10.
X = 100 ?
```

ARM64 should behave identically.

## Files to Watch

Critical files that differ between architectures:
- `emulator/sysdeps.h` - TADBITS definition
- `emulator/regdefs.h` - Register allocation
- `emulator/term.h:342` - MaxSmallNum fix (already done!)

## If You Need Help

See full context in:
- `docs/attempts/ARM64-MINIMAL-PORT.md` - Strategy
- `docs/attempts/ARM64-MACOS-PORT.md` - Previous attempt analysis

---

**Ready to go! Start with the 3 platform files, then test.**
