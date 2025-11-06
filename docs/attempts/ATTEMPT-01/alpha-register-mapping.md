# Alpha Register Mapping

**Date:** 2025-11-05
**Source:** ATTEMPT-01, Phase 1, Step 1.3
**Purpose:** Document Alpha hard register allocation for x86-64 mapping

---

## Alpha HARDREGS Configuration

**File:** `emulator/regdefs.h:32-43`

```c
#ifdef alpha
#define REGISTER1 asm("$23")
#define REGISTER2 asm("$22")
#define REGISTER3 asm("$21")
#define REGISTER4 asm("$20")
#define REGISTER5 asm("$19")
#define REGISTER6 asm("$18")
#define REGISTER7 asm("$17")
#define REGISTER8 asm("$16")
#define REGISTER9 asm("$15")
#define REGISTER10 asm("$14")
#endif
```

---

## Register Allocation in engine.c

**File:** `emulator/engine.c:117-139`

| VM Variable | Register Macro | Alpha Register | Line | Purpose |
|-------------|----------------|----------------|------|---------|
| write_mode  | REGISTER1      | $23            | 131  | Write mode flag (unification) |
| andb        | REGISTER2      | $22            | 139  | Current and-box pointer |
| pc          | REGISTER3      | $21            | 117  | Program counter (code pointer) |
| op          | REGISTER4      | $20            | 119  | Current opcode |
| areg        | REGISTER5      | $19            | 121  | Argument registers pointer |
| yreg        | REGISTER6      | $18            | 123  | Current environment pointer |

**Note:** REGISTER7-10 are defined but UNUSED in the actual code.

---

## Alpha Register Details

### Used Registers (6 total)

**$23 (s7)** - write_mode
- Type: `int`
- Usage: Unification write mode flag
- Callee-saved: Yes

**$22 (s6)** - andb
- Type: `andbox *`
- Usage: Current and-box (concurrency structure)
- Callee-saved: Yes
- Most frequently accessed

**$21 (s5)** - pc
- Type: `code *`
- Usage: Program counter/instruction pointer
- Callee-saved: Yes
- Critical hot path variable

**$20 (s4)** - op
- Type: `opcode`
- Usage: Current instruction opcode
- Callee-saved: Yes
- Fetched every instruction cycle

**$19 (s3)** - areg
- Type: `Term *`
- Usage: Argument register array pointer
- Callee-saved: Yes

**$18 (s2)** - yreg
- Type: `Term *`
- Usage: Current environment (local variables)
- Callee-saved: Yes

### Unused Registers (4 total)

- $17, $16, $15, $14 (REGISTER7-10) - Defined but not assigned to any variable

---

## Alpha Register Conventions (ABI)

Alpha callee-saved registers used:
- $9-$14 (s0-s5): General callee-saved registers
- $15 (fp): Frame pointer (can be used if not needed)
- $16-$21 (a0-a5): Argument registers (saved by callee if needed)
- $22-$25 (t8-t11): Temporary registers (some callee-saved)

**Alpha HARDREGS uses:** $23, $22, $21, $20, $19, $18 (all callee-saved)

**Key Insight:** All 6 allocated registers are callee-saved, ensuring they survive function calls.

---

## Comparison with SPARC

SPARC uses 9 hard registers (from study documents):
- %l7, %l6, %l5, %l4, %l3, %l2 (local registers)
- %i5, %i4, %i3 (input registers)

Alpha uses 6 hard registers:
- $23, $22, $21, $20, $19, $18

**Conclusion:** Alpha uses fewer registers than SPARC but still achieves good performance. This suggests we don't need all 10 available registers for good performance.

---

## x86-64 Target Registers (Preliminary)

x86-64 callee-saved registers available:
- rbx, rbp, r12, r13, r14, r15 (6 total)

**Perfect match!** We have exactly 6 callee-saved registers on x86-64, matching Alpha's usage.

**Proposed x86-64 mapping:**

| VM Variable | Alpha Reg | x86-64 Reg | Notes |
|-------------|-----------|------------|-------|
| write_mode  | $23       | r15d       | 32-bit portion of r15 |
| andb        | $22       | r14        | 64-bit pointer |
| pc          | $21       | r13        | 64-bit pointer |
| op          | $20       | r12d       | 32-bit opcode |
| areg        | $19       | rbx        | 64-bit pointer |
| yreg        | $18       | rbp        | 64-bit pointer (use with care) |

**Alternative if rbp is problematic (frame pointer):**
- Use r11 or r10 as additional scratch registers (caller-saved but fast)
- Or disable frame pointer with -fno-omit-frame-pointer

---

## GCC asm Syntax

Alpha uses:
```c
register type var asm("$23");
```

x86-64 should use:
```c
register type var asm("r15");
```

**Note:** Some GCC versions may require `asm("%r15")` with %, others without. Test both.

---

## Key Findings

1. **Only 6 registers actually used** (not 10)
2. **All are callee-saved** (survive function calls)
3. **x86-64 has exactly 6 callee-saved GPRs** (perfect match)
4. **Most critical:** pc, op, andb (innermost interpreter loop)
5. **Register pressure is moderate** (not all available registers needed)

---

## Related Files

- `emulator/regdefs.h` - Register definitions
- `emulator/engine.c` - Register usage in interpreter
- Study Document: `docs/porting/05-BYTECODE-DISPATCH.md`

---

**Conclusion:** Alpha's register allocation is conservative and well-suited for porting. The 6-register pattern maps cleanly to x86-64's callee-saved registers.
