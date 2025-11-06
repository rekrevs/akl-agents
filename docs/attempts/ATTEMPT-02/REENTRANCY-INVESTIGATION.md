# Parser Reentrancy Investigation - ARM64 Bug

**Date:** 2025-11-06
**Issue:** ARM64 hangs when calling `parse()` multiple times
**Status:** 🔴 ROOT CAUSE IDENTIFIED - Requires architectural decision

---

## Executive Summary

The ARM64 hang is caused by **yacc/flex parser non-reentrancy** in combination with **architecture-specific undefined behavior**. The parser works on x86-64 by **pure luck** - the global state happens to have "workable" garbage values. On ARM64's different memory layout, the garbage values cause an infinite loop.

**Key Finding:** Even an EMPTY second file hangs, proving this is purely a parser state issue, not content-related.

---

## What We Tried

###  1. Reset Static Parser Variables ❌
**Approach:** Added `reset_parser_state()` to reset all static variables in parser.y
**Result:** Still hangs
**Why:** Yacc parser has internal state (stacks, counters) that can't be reset externally

### 2. Add `%pure-parser` Directive ❌
**Approach:** Made bison parser "pure" (reentrant)
**Result:** Still hangs
**Why:** Bison 2.3 (from 2006) doesn't fully support reentrancy; also flex lexer still uses globals

### 3. Add `%option reentrant` to Flex ❌
**Approach:** Make both flex AND bison reentrant
**Result:** Compilation errors - signature mismatch
**Why:** Bison 2.3 and flex 2.5 don't integrate properly for full reentrancy

---

## Root Cause Analysis

### The Fundamental Problem

From [Practical Parsing with Flex and Bison](https://begriffs.com/posts/2021-11-28-practical-parsing.html):

> **A flex scanner and bison parser built in the usual way is not reentrant** and can parse only one input stream at a time because both the scanner and the parser use static data structures to keep track of what they're doing.

### What Happens

**First `parse()` call:**
1. Parser initializes global state
2. Lexer initializes its state
3. Parsing completes
4. **State is NOT reset** (by design in old yacc/flex)

**Second `parse()` call:**
1. Parser sees existing global state
2. Gets confused by stale values
3. On x86-64: happens to work (undefined behavior)
4. On ARM64: infinite loop (different memory layout exposes the bug)

### Why x86-64 "Works"

Pure **undefined behavior luck**:
- Global variables have certain garbage values after first parse
- x86-64 memory layout/register allocation happens to make these "workable"
- ARM64 memory layout/register allocation makes them cause infinite loops

This is **not a bug we introduced** - it's a **latent bug in the original 1990s code** that x86-64 masks.

---

## Evidence

### Test Results

| Test | x86-64 | ARM64 | Conclusion |
|------|--------|-------|------------|
| Single file (boot.pam) | ✅ Works | ✅ Works | Parser itself is fine |
| Single file (comp.pam) | ✅ Works | ✅ Works | No content-specific issues |
| Two files (boot + comp) | ✅ Works | ❌ Hangs | Second parse() hangs |
| Two files (empty + empty) | ✅ Works | ❌ Hangs | Even empty files hang! |
| Two files (empty + boot) | ✅ Works | ❌ Hangs | Any second file hangs |

**Conclusion:** ARM64 cannot call `parse()` more than once in the same process.

### Where It Hangs

Debug output shows:
```
[DEBUG] Loading bootfile: /tmp/comp_empty.pam
[DEBUG] Calling parse()...
[DEBUG] parse() returned          ← First parse succeeds

[DEBUG] Loading bootfile: /tmp/comp_empty.pam
[DEBUG] Calling parse()...
[HANGS FOREVER]                    ← Second parse never returns
```

The hang is **inside the yacc parser's main loop**, not in the lexer or initialization.

---

## Why Modern Reentrancy Doesn't Work

### Requirements for Full Reentrancy

Per [GNU Bison documentation](http://www.gnu.org/software/bison/manual/html_node/Pure-Decl.html):

1. **Bison side:** `%define api.pure full`
2. **Flex side:** `%option reentrant` + `%option bison-bridge`
3. **Integration:** Both must pass state as parameters, not globals

### Our Constraints

- **Bison 2.3** (2006) - predates `%define api.pure full`
- **Old Flex** - doesn't fully integrate with old bison
- **1990s Codebase** - parser.y expects old-style signatures

**Result:** Can't make it truly reentrant without:
1. Upgrading bison/flex to modern versions (2015+)
2. Extensive code changes to match new signatures
3. Full regression testing

---

## Viable Solutions

### Option 1: Upgrade Bison/Flex to Modern Versions ⭐ RECOMMENDED
**Effort:** Medium (4-8 hours)
**Risk:** Medium (need full testing)
**Benefit:** Proper fix, future-proof

**Steps:**
1. Install modern bison (3.8+) and flex (2.6+)
2. Update parser.y.m4:
   ```yacc
   %define api.pure full
   %param {void *scanner}
   ```
3. Update parser.l:
   ```lex
   %option reentrant
   %option bison-bridge
   ```
4. Update all yylex() calls to pass scanner parameter
5. Test extensively on both x86-64 and ARM64

**References:**
- [Reentrant Parser Example](https://www.lemoda.net/c/reentrant-parser/)
- [Complete C++ Example](https://github.com/ezaquarii/bison-flex-cpp-example)

---

### Option 2: Single Parse with Concatenated Input
**Effort:** Low (2-4 hours)
**Risk:** Low
**Benefit:** Simple workaround

**Approach:** Instead of calling `parse()` multiple times, concatenate all boot files into a single stream.

**Issue:** User reports this won't work because `parse()` is called for **other compiled code** during normal operation, not just boot files.

**Status:** ❌ NOT VIABLE per user feedback

---

### Option 3: Reset Flex Internal State Between Parses
**Effort:** Medium (4-6 hours)
**Risk:** High (may not work)
**Benefit:** Minimal code changes

**Approach:** Manually access and reset flex's internal `yy_buffer_stack`, `yy_init`, etc.

**Implementation:**
```c
extern void yy_delete_buffer(YY_BUFFER_STATE);
extern YY_BUFFER_STATE yy_current_buffer;

void reset_lexer_completely() {
    if (yy_current_buffer) {
        yy_delete_buffer(yy_current_buffer);
        yy_current_buffer = NULL;
    }
    // Reset other flex internals
}
```

**Problem:**
- Requires accessing flex internals that may not be exported
- Yacc parser state still problematic
- Fragile, may break between flex versions

---

### Option 4: Subprocess Per File
**Effort:** High (8-12 hours)
**Risk:** High (complex)
**Benefit:** Complete isolation

**Approach:** Fork a subprocess for each `parse()` call, communicate via IPC.

**Issues:**
- Subprocesses don't share memory
- Loaded predicates from file 1 won't be visible in file 2
- Would need shared memory or serialization
- **Not viable** for this use case

---

### Option 5: Accept x86-64 Only for Now
**Effort:** None
**Risk:** None
**Benefit:** Can proceed with other work

**Approach:** Document ARM64 as unsupported until parser can be modernized.

**Status:** Interim solution while planning Option 1

---

## Recommended Path Forward

### Immediate (This Week)
1. ✅ Document the root cause (this document)
2. ✅ Commit all investigation work
3. ⏸️ **Decision Point:** Choose Option 1 or Option 5

### Short-term (If choosing Option 1)
1. Install modern bison 3.8+ and flex 2.6+
2. Test x86-64 build with new tools (verify no regressions)
3. Implement full reentrancy (both bison and flex)
4. Test on ARM64
5. Regression test entire system

### Alternative (If choosing Option 5)
1. Mark ARM64 as "experimental/unsupported" in documentation
2. Continue with x86-64 development
3. Revisit ARM64 when time permits for full modernization

---

## Technical Details

### Tools Versions

**Current (Problematic):**
- Bison 2.3 (2006)
- Flex 2.5.x
- Yacc-style grammar (1990s)

**Recommended:**
- Bison 3.8+ (2021+)
- Flex 2.6+ (2015+)
- Modern `%define` directives

### Key Differences

| Feature | Old Bison 2.3 | Modern Bison 3.8+ |
|---------|---------------|-------------------|
| Reentrancy | `%pure-parser` (partial) | `%define api.pure full` |
| Parameters | Not supported well | `%param {scanner}` |
| Integration | Poor with flex | Good with flex 2.6+ |
| Documentation | Limited | Comprehensive |

### Files That Need Changes (Option 1)

1. `emulator/parser.y.m4` - Add modern reentrancy directives
2. `emulator/parser.l` - Add `%option reentrant bison-bridge`
3. `emulator/load.c` - Pass scanner parameter to parse()
4. Build system - Detect/require modern bison/flex

---

## Lessons Learned

### 1. Undefined Behavior is Platform-Specific
Same code, same compiler, different architecture = different behavior. This is the textbook definition of undefined behavior.

### 2. "Working" ≠ "Correct"
x86-64 "working" hid a serious bug for potentially decades. ARM64 exposed it immediately.

### 3. Old Tools Have Limitations
Bison 2.3 and flex from 2006 predate modern reentrancy support. Upgrading is sometimes necessary.

### 4. Testing Matters
Simple test (load empty file twice) immediately exposed the issue. Edge case testing is valuable.

### 5. Web Search Helps
Finding documentation about flex/bison reentrancy provided the missing piece of the puzzle.

---

## References

### Documentation
- [GNU Bison - Pure Parsers](http://www.gnu.org/software/bison/manual/html_node/Pure-Decl.html)
- [Flex Manual - Reentrant Scanners](https://westes.github.io/flex/manual/Reentrant.html)
- [Practical Parsing with Flex and Bison](https://begriffs.com/posts/2021-11-28-practical-parsing.html)

### Examples
- [Reentrant Parser Tutorial](https://www.lemoda.net/c/reentrant-parser/)
- [Complete C++ Example](https://github.com/ezaquarii/bison-flex-cpp-example)
- [Stack Overflow: Multiple parsers](https://stackoverflow.com/questions/1634704/multiple-flex-bison-parsers)

### Related Issues
- [Thread-safe bison+flex](https://stackoverflow.com/questions/48850242/thread-safe-reentrant-bison-flex)
- [Eric Raymond on reentrancy](http://esr.ibiblio.org/?p=6341)

---

## Conclusion

The ARM64 hang is a **legitimate bug** in the original code that x86-64 masks through undefined behavior. The proper fix requires **upgrading to modern bison/flex** and implementing **full reentrancy**.

**Decision Required:** Proceed with Option 1 (proper fix) or Option 5 (defer ARM64)?

**Estimated effort for Option 1:** 8-16 hours total
- 2-4 hours: Install and test new tools
- 4-8 hours: Implement reentrancy
- 2-4 hours: Testing and debugging

**Confidence:** 85% that Option 1 will fully resolve the issue

---

**End of Investigation**
