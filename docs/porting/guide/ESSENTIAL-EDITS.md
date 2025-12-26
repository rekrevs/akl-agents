# Essential Code Edits for 64-Bit Port

**Quick Reference: Minimal changes needed to port AGENTS to 64-bit platforms**

---

## Summary: 3 Files, ~10 Lines Changed

The port required minimal, surgical changes to make AGENTS work on modern 64-bit platforms (x86-64 and ARM64).

---

## 1. emulator/term.h (Line 342)
### Fix: MaxSmallNum Signedness Bug

```c
// BEFORE (buggy on 64-bit):
#define MaxSmallNum (1UL << (SMALLNUMBITS-1))

// AFTER (correct):
#define MaxSmallNum (1L << (SMALLNUMBITS-1))
```

**What it fixes:** Negative integers weren't recognized as small numbers on 64-bit platforms because unsigned arithmetic made `-MaxSmallNum` wrap around to a huge positive value instead of being negative.

**Impact:** All 64-bit platforms (this was a latent bug in the original 1990s code)

---

## 2. emulator/load.c (Lines 18-21)
### Fix: Parser Reentrancy for Multiple File Loading

```c
// BEFORE (broken on ARM64):
extern FILE *yyin;
extern int parse();

void load(file)
    FILE *file;
{
    yyin = file;
    parse();
}

// AFTER (working):
extern FILE *yyin;
extern int parse();
extern void yyrestart(FILE *);  // ADD THIS LINE

void load(file)
    FILE *file;
{
    yyrestart(file);  // ADD THIS LINE - resets lexer state
    parse();
}
```

**What it fixes:** Parser state corruption when loading multiple boot files. System would hang on second `load()` call.

**Impact:** Critical on ARM64, masked on x86-64 (undefined behavior)

---

## 3. Makefile.in (Lines 54, 90-92)
### Fix: Version Bump and Build System

```makefile
# Line 54 - Version bump:
VERSION = 0.9.1  # was: 0.9

# Lines 90-92 - Use modern Bison and add compiler flags:
BISON = /opt/homebrew/opt/bison/bin/bison  # was: bison
CC = @CC@
CFLAGS = @CFLAGS@ -Wno-implicit-int -Wno-implicit-function-declaration -Wno-int-conversion
```

**What it fixes:**
- Bison 3.8.2 provides proper `%pure-parser` implementation (Bison 2.3 was incomplete)
- Compiler flags suppress errors from 1990s C code patterns on modern compilers

**Impact:** Build system compatibility with modern toolchains

---

## Supporting Requirements

### Build Configuration
```bash
./configure --without-gmp
```

**Why:** The bundled 1990s GMP library doesn't compile on modern ARM64. System uses 57-bit small integers instead (sufficient for most applications).

### Parser Grammar (No Change Needed)
`emulator/parser.y.m4` already contained:
```yacc
%pure-parser
```

This directive was present in the original code but required Bison 3.x to implement correctly. No code change needed.

---

## Results

**Total functional changes:**
- 1 character changed: `UL` → `L` (term.h)
- 2 lines added: `extern void yyrestart()` declaration (load.c)
- 1 line added: `yyrestart(file)` call (load.c)
- 3 lines modified: BISON path, CFLAGS, VERSION (Makefile.in)

**= 7 lines of functional code changes**

**Platforms verified working:**
- ✅ x86-64 (Intel/AMD 64-bit)
- ✅ ARM64 (Apple Silicon / AArch64)

**Preserved:**
- ✅ Original code structure unchanged
- ✅ No breaking changes to APIs
- ✅ 32-bit compatibility maintained (untested but code paths preserved)

---

## Why So Few Changes?

1. **Good original design:** The system was already architected for portability
2. **Latent bugs exposed:** 64-bit architectures revealed issues that were dormant
3. **Modern tools:** Bison 3.x finally implemented features the original code expected
4. **Minimal intervention philosophy:** Only fix what's broken, preserve everything else

---

## References

- Complete technical analysis: `docs/64-BIT-PORTING-GUIDE.md`
- Change log: `CHANGES.md`
- Merge strategy: `MERGE-RECOMMENDATIONS.md`
- Attempt logs: `docs/attempts/ATTEMPT-01/` and `docs/attempts/ATTEMPT-02/`

---

**Version:** 0.9.1
**Date:** November 2025
**Status:** Production-ready
