# ARM64 / Apple Silicon Porting Assessment

**Date:** 2025-11-05
**Context:** After successful x86-64 port
**Question:** What would it take to port AGENTS to ARM64 (including Apple Silicon M1/M2/M3)?

---

## Quick Answer

**Very feasible! Estimated effort: 1-2 hours**

ARM64 is actually an **easier target** than x86-64 in some ways:
- More registers available (30 general-purpose vs 16 on x86-64)
- Clean, modern ISA (no legacy baggage)
- Excellent GCC/Clang support
- Same 64-bit addressing model

---

## Platform Analysis

### ARM64 Architecture

**Register Set:**
- **31 general-purpose 64-bit registers** (x0-x30, plus sp)
- x0-x7: Argument passing
- x8: Indirect result location
- x9-x15: Temporary/caller-saved
- **x19-x28: Callee-saved** ← Perfect for our hard registers!
- x29: Frame pointer
- x30: Link register

**For AGENTS we need 6 registers:**
- Plenty available in x19-x28 range (10 callee-saved!)
- Much easier than x86-64 (which has exactly 6)

### Apple Silicon Specifics

**M-series chips (M1/M2/M3/M4):**
- Full ARM64v8.4 or later
- Rosetta 2 provides x86-64 emulation (but native is better!)
- Excellent compiler support (Apple Clang)
- macOS requires code signing (but not for local builds)

---

## Required Changes

### 1. Platform Detection (emulator/sysdeps.h)

Add ARM64 detection:

```c
#if defined(__x86_64__) || defined(__amd64__)
#define TADBITS 64
#endif

#if defined(__aarch64__) || defined(__arm64__)
#define TADBITS 64
#endif
```

**That's it for platform detection!** Everything else derives automatically.

### 2. Register Allocation (emulator/regdefs.h)

Map to ARM64 callee-saved registers:

```c
#if defined(__aarch64__) || defined(__arm64__)
#define REGISTER1 asm("x19")   // write_mode
#define REGISTER2 asm("x20")   // andb
#define REGISTER3 asm("x21")   // pc
#define REGISTER4 asm("x22")   // op
#define REGISTER5 asm("x23")   // areg
#define REGISTER6 asm("x24")   // yreg
#define REGISTER7 asm("x25")   // spare
#define REGISTER8 asm("x26")   // spare
#define REGISTER9 asm("x27")   // spare
#define REGISTER10 asm("x28")  // spare
#endif
```

**Note:** We have 4 extra spares (x25-x28) compared to x86-64!

### 3. Build System (config.guess)

Add ARM64 recognition:

```sh
aarch64:Linux:*:*)
    echo aarch64-unknown-linux-gnu
    exit 0 ;;

arm64:Darwin:*:*)
    echo arm64-apple-darwin
    exit 0 ;;
```

### 4. That's It!

**Total new code: ~15 lines** (even less than x86-64!)

---

## What We Already Have

The x86-64 port already fixed:
- ✅ MaxSmallNum signedness bug (benefits ARM64!)
- ✅ Parser bugs (all platforms)
- ✅ Deprecated functions (gcvt, getwd)
- ✅ Build system improvements

ARM64 inherits all these fixes.

---

## Expected Challenges

### Minimal (If Any)

**1. Compiler Differences**

GCC vs Clang (Apple uses Clang):
- Both support `asm("register")` syntax
- Might need to test on both compilers
- Likely "just works"

**2. Calling Convention**

ARM64 has different calling convention than x86-64:
- Arguments in x0-x7 (vs rdi,rsi,rdx,rcx,r8,r9)
- But callee-saved registers are well-defined
- AGENTS doesn't rely on argument passing order

**3. Memory Ordering**

ARM has weaker memory model than x86-64:
- But AGENTS is single-threaded in emulator core
- No atomics in critical paths
- Should be fine

**4. macOS Specifics**

On Apple Silicon macOS:
- May need code signing for distribution (not for dev)
- Homebrew paths different (/opt/homebrew vs /usr/local)
- But local builds should work fine

### Advantages Over x86-64

**1. More Registers**

10 callee-saved vs 6 on x86-64:
- Could use extras for optimization
- Less register pressure
- Potentially better performance

**2. Cleaner ISA**

ARM64 is modern, consistent design:
- No x86 legacy quirks
- Better compiler optimization
- More predictable behavior

**3. Better Branch Prediction**

Apple Silicon M-series:
- Excellent branch predictor
- Wide execution units
- Could be faster than x86-64!

---

## Porting Strategy

### Option A: Quick Port (1 hour)

1. Add ARM64 detection (sysdeps.h) - 5 min
2. Add register mapping (regdefs.h) - 10 min
3. Update config.guess - 5 min
4. Test build - 20 min
5. Test runtime - 20 min

**Total: ~1 hour** for basic port

### Option B: Optimized Port (2-4 hours)

Same as Option A, plus:
- Benchmark vs x86-64
- Experiment with using extra registers
- Profile for ARM-specific optimizations
- Test on both Linux and macOS

### Option C: Follow x86-64 Process (4-6 hours)

Complete documentation:
- ATTEMPT-02 directory
- Full investigation logs
- Performance comparison
- Add to book as case study

---

## Testing Requirements

### Hardware Needed

**Apple Silicon:**
- Any M1/M2/M3/M4 Mac
- macOS 11.0 or later
- Xcode command line tools

**ARM64 Linux:**
- Raspberry Pi 4/5 (64-bit mode)
- ARM server (AWS Graviton, etc.)
- QEMU ARM64 emulation

**Or:**
- GitHub Actions has ARM64 runners
- Can test without physical hardware

---

## Estimated Compatibility

Based on x86-64 experience:

| Aspect | Likelihood | Reasoning |
|--------|-----------|-----------|
| Compilation | 99% | ARM64 well-supported |
| Linking | 99% | Same issues as x86-64 (solved) |
| Runtime | 95% | Similar to x86-64 |
| Full boot | 95% | Signedness fix applies |
| REPL | 95% | If x86-64 works, ARM64 should |
| Performance | Better? | More registers, modern ISA |

**Overall confidence: VERY HIGH**

---

## Code Diff Estimate

```diff
# emulator/sysdeps.h
+#if defined(__aarch64__) || defined(__arm64__)
+#define TADBITS 64
+#endif

# emulator/regdefs.h
+#if defined(__aarch64__) || defined(__arm64__)
+#define REGISTER1 asm("x19")   // write_mode
+#define REGISTER2 asm("x20")   // andb
+#define REGISTER3 asm("x21")   // pc
+#define REGISTER4 asm("x22")   // op
+#define REGISTER5 asm("x23")   // areg
+#define REGISTER6 asm("x24")   // yreg
+#define REGISTER7 asm("x25")   // spare
+#define REGISTER8 asm("x26")   // spare
+#define REGISTER9 asm("x27")   // spare
+#define REGISTER10 asm("x28")  // spare
+#endif

# config.guess
+aarch64:Linux:*:*)
+    echo aarch64-unknown-linux-gnu
+    exit 0 ;;
+arm64:Darwin:*:*)
+    echo arm64-apple-darwin
+    exit 0 ;;
```

**Total: ~20 lines** (including blank lines and comments)

---

## Risks & Mitigation

### Low Risk Items

**1. Register allocation**
- Risk: GCC might not honor register variables
- Mitigation: ARM64 GCC is mature, should work
- Fallback: Works without register allocation (just slower)

**2. Calling convention**
- Risk: ABI violations
- Mitigation: We use callee-saved (safe)
- Test: Same runtime tests as x86-64

**3. Alignment**
- Risk: ARM64 stricter about alignment
- Mitigation: WORDALIGNMENT=8 already correct
- Verify: No bus errors in testing

### Already Solved

Thanks to x86-64 port:
- ✅ Signedness bug fixed
- ✅ Parser works
- ✅ Build system modernized
- ✅ Deprecated functions replaced

---

## Performance Expectations

### Why ARM64 Might Be Faster

**1. More Registers**
- Less spilling to memory
- Better for register-intensive VM

**2. Modern Architecture**
- Out-of-order execution
- Better branch prediction (especially M-series)

**3. Simpler Instructions**
- RISC design
- More predictable performance

### Benchmark Ideas

```bash
# Simple timing test
time ./agents -b boot.pam <<EOF
X is 10000000 * 10000000.
EOF

# Compare:
# - x86-64 Intel
# - x86-64 AMD
# - ARM64 M-series
# - ARM64 Linux (Raspberry Pi / Graviton)
```

---

## Recommended Approach

### For You

Given your successful x86-64 port experience:

**Option 1: Quick & Dirty (Recommended First)**

1. Checkout x86-64-improvements branch
2. Add ARM64 support (20 lines)
3. Test on ARM64 hardware
4. Commit if it works
5. Document results

**If you have ARM64 access:** 2-3 hours total
**If testing via CI:** 4-5 hours (iteration time)

**Option 2: Full ATTEMPT-02 (For Book)**

Same process as x86-64:
- ATTEMPT-02 directory
- Full documentation
- Investigation logs
- Compare performance
- Add to book chapters

**Time:** 6-8 hours (including documentation)

### Starting Point

```bash
git checkout claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP
# Add ARM64 support
# Test
# Commit
```

Or for full attempt:

```bash
git checkout -b claude/arm64-attempt-02-011CUoHSMTFawkiKfVsuVuPP
# Follow ATTEMPT-01 methodology
# Document everything
```

---

## What You'd Learn

**New Insights:**

1. **Multi-platform development**
   - Managing multiple architectures
   - Conditional compilation patterns
   - Cross-platform testing

2. **ARM64 specifics**
   - Register usage patterns
   - ABI differences
   - Performance characteristics

3. **Comparative analysis**
   - x86-64 vs ARM64 performance
   - Compiler behavior differences
   - Architecture trade-offs

**Book Value:**

- Chapter: "ARM64 Port - The Second Platform"
- Comparison with x86-64 experience
- Multi-architecture strategy
- Performance analysis

---

## Bottom Line

**Difficulty:** EASY (easier than x86-64!)

**Time Required:**
- Quick port: 1-2 hours
- Tested port: 2-4 hours
- Documented case study: 6-8 hours

**Success Probability:** 95%+

**Value:**
- Native Apple Silicon support
- ARM server support (AWS Graviton, etc.)
- Raspberry Pi compatibility
- Performance comparison data
- Proves portability approach

**Recommendation:** Do it! It's a perfect follow-up to x86-64, and will prove the methodology scales to multiple architectures.

Plus, ARM64 is increasingly important (Apple Silicon, cloud ARM instances, embedded systems).

---

## Next Steps If You Want To Proceed

1. **Get ARM64 access** (Mac M-series, Raspberry Pi, or cloud)
2. **Clone the repo** on ARM64 system
3. **Checkout x86-64-improvements branch**
4. **Add 20 lines** of ARM64 support
5. **Test and commit**
6. **Celebrate** having SPARC, Alpha, x86-64, AND ARM64 support!

Want me to help with the ARM64 port? I can:
- Write the exact code changes needed
- Create ATTEMPT-02 structure
- Add it to the book
- All in the same session if you have ARM64 hardware to test on!
