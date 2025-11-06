# Alpha Configuration Extracted

**Date:** 2025-11-05
**Source:** ATTEMPT-01, Phase 1, Step 1.2
**Purpose:** Document Alpha 64-bit port configuration for x86-64 mapping

---

## Alpha Platform Detection

**File:** `emulator/sysdeps.h:1-3`

```c
#ifdef __alpha__
#define TADBITS 64
#endif
```

**Note:** This is the ONLY Alpha-specific configuration in sysdeps.h.

---

## Key Configuration Values

### TADBITS (Tag Address Bits)
- **Value:** 64
- **Source:** `emulator/sysdeps.h:2`
- **Default (32-bit):** 32 (`emulator/term.h:70`)
- **Meaning:** Alpha uses 64-bit tagged addresses

### PTR_ORG (Pointer Origin/Base)
- **Value:** 0 (uses default)
- **Source:** `emulator/term.h:line` (default not overridden)
- **Meaning:** No pointer adjustment needed (unlike HP-PA which uses 0x40000000)

### WORDALIGNMENT
- **Value:** 8 (derived)
- **Source:** `emulator/term.h:79` - `#define WORDALIGNMENT (TADBITS/8)`
- **Calculation:** 64/8 = 8 bytes
- **Meaning:** 8-byte memory alignment required

---

## Derived Configuration

Because TADBITS=64, several other values are automatically adjusted:

### SMALLNUMBITS (Small Integer Bits)
- **Formula:** TADBITS - 6
- **Value:** 64 - 6 = 58 bits
- **Source:** `emulator/term.h:85`
- **Meaning:** Small integers can be 58 bits (vs 26 bits on 32-bit platforms)

### MarkBitMask (GC Mark Bit)
- **Formula:** 0x1 << (TADBITS-1)
- **Value:** Bit 63 (highest bit)
- **Source:** `emulator/term.h:98`
- **Meaning:** Mark bit is in the MSB for GC

---

## Comparison: Alpha vs SPARC (32-bit)

| Setting | SPARC (32-bit) | Alpha (64-bit) | Ratio |
|---------|----------------|----------------|-------|
| TADBITS | 32 | 64 | 2x |
| WORDALIGNMENT | 4 | 8 | 2x |
| SMALLNUMBITS | 26 | 58 | 2.2x |
| PTR_ORG | 0 | 0 | same |

---

## Key Insight

**The Alpha port uses a MINIMAL configuration change:** Just setting TADBITS=64 causes all other 64-bit adaptations to happen automatically through macros.

**Implication for x86-64:** We should be able to use the same approach:
```c
#if defined(__x86_64__) || defined(__amd64__)
#define TADBITS 64
#endif
```

And PTR_ORG=0, WORDALIGNMENT=8 will be derived automatically.

---

## No Special Cases Found

The Alpha configuration does NOT use:
- ❌ `OptionalWordAlign()` (like Macintosh does)
- ❌ Custom `PTR_ORG` (like HP-PA and AIX do)
- ❌ Any `HAS_NO_*` compatibility flags
- ❌ Any special includes or extern declarations

This suggests a **clean, straightforward 64-bit port**.

---

## Related Files

- `emulator/sysdeps.h` - Platform configuration
- `emulator/term.h` - Core type definitions and derived macros
- Study Document: `docs/porting/03-PLATFORM-DEPENDENCIES.md`

---

**Conclusion:** Alpha configuration is remarkably simple - just TADBITS=64, everything else is derived. This bodes very well for x86-64 porting.
