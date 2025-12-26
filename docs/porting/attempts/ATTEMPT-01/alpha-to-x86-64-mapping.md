# Alpha to x86-64 Porting Mapping

**Date:** 2025-11-05
**Source:** ATTEMPT-01, Phase 2, Step 2.1
**Purpose:** Systematic mapping guide from Alpha patterns to x86-64 equivalents

---

## Table of Contents

1. [Platform Macros Mapping](#platform-macros-mapping)
2. [Register Mapping](#register-mapping)
3. [ASM Syntax Mapping](#asm-syntax-mapping)
4. [Configuration Values](#configuration-values)
5. [Implementation Plan](#implementation-plan)

---

## 1. Platform Macros Mapping

### Platform Detection

| Alpha Pattern | x86-64 Pattern | Rationale |
|---------------|----------------|-----------|
| `#ifdef __alpha__` | `#if defined(__x86_64__) \|\| defined(__amd64__)` | GCC standard macros for x86-64 architecture |

**Notes:**
- `__x86_64__` is the primary GCC macro
- `__amd64__` is an alias (AMD64 = x86-64)
- Use both for maximum compatibility
- Intel compiler also recognizes these

---

### Configuration Macros

| Macro | Alpha Value | x86-64 Value | Source/Derivation |
|-------|-------------|--------------|-------------------|
| `TADBITS` | 64 | 64 | Explicit (both 64-bit) |
| `PTR_ORG` | 0 | 0 | Default (no override needed) |
| `WORDALIGNMENT` | 8 | 8 | Derived: TADBITS/8 = 64/8 = 8 |
| `SMALLNUMBITS` | 58 | 58 | Derived: TADBITS-6 = 64-6 = 58 |
| `MarkBitMask` | Bit 63 | Bit 63 | Derived: 1<<(TADBITS-1) |

**Key Insight:** Only TADBITS needs to be set explicitly. All other values are automatically derived by existing macros in term.h.

---

## 2. Register Mapping

### Alpha → x86-64 Register Assignment

| VM Variable | Alpha Reg | x86-64 Reg | Type | Notes |
|-------------|-----------|------------|------|-------|
| write_mode  | $23 (s7) | r15d | int (32-bit) | Use 32-bit portion of r15 |
| andb        | $22 (s6) | r14 | andbox* (64-bit) | Most frequently accessed |
| pc          | $21 (s5) | r13 | code* (64-bit) | Program counter |
| op          | $20 (s4) | r12d | opcode (32-bit) | Use 32-bit portion of r12 |
| areg        | $19 (s3) | rbx | Term* (64-bit) | Argument registers |
| yreg        | $18 (s2) | rbp | Term* (64-bit) | Environment (see notes) |

**Register Choice Rationale:**

**r12-r15:** Callee-saved, 64-bit, available in all x86-64 modes
- r15/r15d: write_mode (int flag)
- r14: andb (critical hot path)
- r13: pc (critical hot path)
- r12/r12d: op (opcode, fetched every instruction)

**rbx:** Traditional callee-saved register, widely used
- areg: Argument register array pointer

**rbp:** Callee-saved, but traditionally frame pointer
- yreg: Environment pointer
- **IMPORTANT:** May require `-fno-omit-frame-pointer` flag OR alternative register
- **Alternative:** Use r11 (caller-saved but fast) if rbp causes issues

---

### Register Properties Comparison

| Property | Alpha ($23-$18) | x86-64 (r15-r12, rbx, rbp) |
|----------|-----------------|----------------------------|
| Callee-saved | Yes (all 6) | Yes (all 6) |
| 64-bit capable | Yes | Yes |
| Available in all modes | Yes | Yes |
| Function call safe | Yes | Yes |
| Count | 6 | 6 (perfect match!) |

---

### Alternative Register Allocation (if rbp is problematic)

| VM Variable | Primary Choice | Alternative | Notes |
|-------------|----------------|-------------|-------|
| write_mode  | r15d | r11d | r11 is scratch but faster |
| andb        | r14 | r14 | Keep (most critical) |
| pc          | r13 | r13 | Keep (most critical) |
| op          | r12d | r10d | r10 is scratch but faster |
| areg        | rbx | rbx | Keep (standard) |
| yreg        | rbp | r9 | r9 is caller-saved |

**Use Alternative If:**
- Compilation fails with "rbp reserved" errors
- Debugging requires frame pointers
- Profiling tools need frame pointers

---

## 3. ASM Syntax Mapping

### GCC Register Constraint Syntax

| Alpha Syntax | x86-64 Syntax | Notes |
|--------------|---------------|-------|
| `asm("$23")` | `asm("r15")` | Without % prefix |
| `asm("$22")` | `asm("r14")` | OR `asm("%r15")` with % |
| etc. | etc. | Test both if needed |

**Examples:**

**Alpha:**
```c
register int write_mode asm("$23");
register andbox *andb asm("$22");
```

**x86-64 Option 1 (without %):**
```c
register int write_mode asm("r15");
register andbox *andb asm("r14");
```

**x86-64 Option 2 (with %, if Option 1 fails):**
```c
register int write_mode asm("%r15");
register andbox *andb asm("%r14");
```

**Testing:** Try Option 1 first (matches Alpha style). If compiler errors occur, try Option 2.

---

### 32-bit vs 64-bit Register Names

For 32-bit values in 64-bit registers:

| Variable | Type | 64-bit Reg | 32-bit Name | Syntax |
|----------|------|------------|-------------|--------|
| write_mode | int | r15 | r15d | `asm("r15")` or `asm("r15d")` |
| op | opcode (u16/int) | r12 | r12d | `asm("r12")` or `asm("r12d")` |

**Note:** GCC usually accepts either `asm("r15")` or `asm("r15d")` for int types - it will use the appropriate portion automatically. Test to confirm.

---

## 4. Configuration Values

### Explicit Configuration Needed

**File:** `emulator/sysdeps.h`

**Add after Alpha block:**

```c
#if defined(__x86_64__) || defined(__amd64__)
#define TADBITS 64
#endif
```

That's it! Everything else is derived.

---

### Derived Values (Automatic)

**File:** `emulator/term.h` (no changes needed)

These are automatically calculated from TADBITS:

```c
#ifndef WORDALIGNMENT
#define WORDALIGNMENT (TADBITS/8)  // = 8 for 64-bit
#endif

#define SMALLNUMBITS (TADBITS-6)   // = 58 for 64-bit

#define MarkBitMask ((uword)0x1 << (TADBITS-1))  // = bit 63 for 64-bit
```

**Verification:** These should work correctly once TADBITS=64 is set.

---

###No Special Macros Needed

**DO NOT need to define:**
- ❌ `OptionalWordAlign()` (like Macintosh) - Alpha doesn't use it
- ❌ `HAS_NO_GCVT` (like MIPS) - Modern systems have it
- ❌ `HAS_NO_GETWD` (like SYS5) - Will replace with getcwd anyway
- ❌ Custom `PTR_ORG` (like HP-PA) - Use default 0

**Reason:** Alpha proved these aren't needed for 64-bit ports.

---

## 5. Implementation Plan

### File: emulator/sysdeps.h

**Location:** After Alpha block (line 3)

**Code to Add:**
```c
#if defined(__x86_64__) || defined(__amd64__)
#define TADBITS 64
#endif
```

**Verification:**
```bash
gcc -E emulator/sysdeps.h -o /tmp/sysdeps.i
grep -A 2 "x86_64" /tmp/sysdeps.i
```

---

### File: emulator/regdefs.h

**Location:** After Alpha block (line 43)

**Code to Add:**
```c
#if defined(__x86_64__) || defined(__amd64__)
#define REGISTER1 asm("r15")
#define REGISTER2 asm("r14")
#define REGISTER3 asm("r13")
#define REGISTER4 asm("r12")
#define REGISTER5 asm("rbx")
#define REGISTER6 asm("rbp")
#define REGISTER7 asm("r11")  // Spare (unused currently)
#define REGISTER8 asm("r10")  // Spare (unused currently)
#define REGISTER9 asm("r9")   // Spare (unused currently)
#define REGISTER10 asm("r8")  // Spare (unused currently)
#endif
```

**Notes:**
- REGISTER7-10 defined for future use but currently unused (like Alpha)
- Can use for optimization later
- rbp (REGISTER6) may need alternative - see Section 2

**Verification:**
```bash
echo 'register int x asm("r15");' | gcc -c -x c - -o /tmp/test.o 2>&1
```
Should succeed with no errors.

---

### File: configure.in

**Changes:** **NONE**

Alpha doesn't need configure.in changes, so x86-64 shouldn't either.

**Rationale:**
- x86-64 is well-supported by modern GCC
- Default compiler flags work fine
- config.guess/config.sub already recognize x86-64

---

## Summary Checklist

When implementing x86-64 support:

- [ ] Add x86-64 detection to sysdeps.h (1 block, 3 lines)
- [ ] Add x86-64 registers to regdefs.h (1 block, ~13 lines)
- [ ] No configure.in changes needed
- [ ] No term.h changes needed (derivations automatic)
- [ ] No .c file changes needed (no runtime conditionals)
- [ ] Test compilation with verification commands
- [ ] Optionally test rbp alternative if needed

**Total Code Changes:** ~16 lines in 2 files

**Expected Build Time:** <5 minutes once code is added

**Confidence:** HIGH - Direct Alpha pattern reuse

---

## Potential Issues and Solutions

### Issue 1: Register Constraints Rejected

**Symptom:** `error: impossible register constraint`

**Solutions:**
1. Try syntax with `%`: `asm("%r15")` instead of `asm("r15")`
2. Check GCC version: `gcc --version` (need 4.x+ for full support)
3. Try alternative register allocation (Section 2)

---

### Issue 2: rbp Causes Frame Pointer Conflicts

**Symptom:** `error: bp cannot be used in asm`

**Solutions:**
1. Add compiler flag: `-fno-omit-frame-pointer`
2. OR use alternative allocation with r9 for yreg (Section 2)

---

### Issue 3: 32-bit vs 64-bit Naming

**Symptom:** Type mismatch warnings

**Solutions:**
1. Explicitly use 32-bit names: `asm("r15d")` for int
2. OR cast appropriately in code
3. Check sizeof(int) == 4 and sizeof(void*) == 8

---

## Related Documents

- **Phase 1 Output:**
  - `alpha-config-extracted.md` - Alpha configuration analysis
  - `alpha-register-mapping.md` - Alpha register details

- **Study Documents:**
  - `docs/porting/03-PLATFORM-DEPENDENCIES.md` - Platform overview
  - `docs/porting/05-BYTECODE-DISPATCH.md` - Register allocation rationale

---

**Status:** Ready for implementation in Phase 3
**Next Step:** Create verification scripts (Step 2.2)
