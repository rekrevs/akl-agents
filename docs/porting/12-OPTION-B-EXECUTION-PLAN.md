# Option B: Direct Modern Port - Detailed Execution Plan

**Document 12 of 13**
**Version:** 1.0
**Date:** 2025-11-05
**Purpose:** Step-by-step execution plan for Claude Code to port AGENTS v0.9 to x86-64/ARM64

---

## Table of Contents

1. [Overview](#overview)
2. [Execution Principles](#execution-principles)
3. [Phase 1: Alpha Port Analysis](#phase-1-alpha-port-analysis)
4. [Phase 2: Pattern Extraction](#phase-2-pattern-extraction)
5. [Phase 3: x86-64 Configuration](#phase-3-x86-64-configuration)
6. [Phase 4: Incremental Implementation](#phase-4-incremental-implementation)
7. [Phase 5: Build and Test](#phase-5-build-and-test)
8. [Phase 6: ARM64 Adaptation](#phase-6-arm64-adaptation)
9. [Verification Commands](#verification-commands)
10. [Rollback Procedures](#rollback-procedures)

---

## 1. Overview

This document provides a detailed, step-by-step execution plan for porting AGENTS v0.9 to modern x86-64 and ARM64 architectures using the **Direct Modern Port** approach (Option B).

**Strategy:** Analyze the existing Alpha 64-bit port, extract patterns, and systematically apply them to x86-64, then ARM64.

**Key Success Factor:** The Alpha port is a proven 64-bit implementation that serves as our blueprint.

**Expected Outcome:** Native x86-64 build that passes >95% of DejaGnu tests, followed by ARM64 port.

---

## 2. Execution Principles

### 2.1 Small, Verifiable Steps

Every step must:
- Be completable in a single Claude Code session (<10 minutes)
- Have a clear verification command
- Have explicit success criteria
- Be reversible if it fails

### 2.2 Verification-First Approach

Before moving to the next step:
1. Run verification command
2. Check success criteria
3. Document results
4. Only proceed if criteria met

### 2.3 Document Everything

For each step, document:
- What was changed
- Why it was changed
- Verification output
- Any unexpected behavior
- Decisions made

### 2.4 Fail Fast, Learn Fast

If a step fails:
1. Document the failure in ATTEMPT-XX-LOG.md
2. Analyze the root cause
3. Update the plan if needed
4. Don't proceed until issue resolved or documented as known limitation

---

## 3. Phase 1: Alpha Port Analysis

**Goal:** Extract complete knowledge of how Alpha port differs from SPARC.

### Step 1.1: Identify Alpha-Specific Files

**Action:**
```bash
grep -r "alpha" --include="*.h" --include="*.c" emulator/ configure.in
grep -r "ALPHA" --include="*.h" --include="*.c" emulator/ configure.in
grep -r "__alpha" --include="*.h" --include="*.c" emulator/
```

**Expected Files:**
- `emulator/sysdeps.h` - Platform configuration
- `emulator/regdefs.h` - Register definitions
- `configure.in` - Build configuration

**Verification:**
```bash
# Count Alpha references
grep -rc "alpha\|ALPHA\|__alpha" emulator/ configure.in | grep -v ":0$"
```

**Success Criteria:**
- Found all files with Alpha-specific code
- No files missed (cross-reference with `find emulator/ -name "*.h" -o -name "*.c"`)

**Document In:** `ATTEMPT-01-LOG.md` under "Phase 1.1 Results"

---

### Step 1.2: Extract Alpha Configuration from sysdeps.h

**Action:** Read and document Alpha platform configuration.

**Command:**
```bash
grep -A 20 "alpha\|ALPHA\|__alpha" emulator/sysdeps.h
```

**What to Extract:**
- TADBITS value (should be 64)
- PTR_ORG value (should be 0)
- WORDALIGNMENT value (should be 8)
- Any other Alpha-specific macros

**Expected Pattern:**
```c
#ifdef __alpha
#define TADBITS 64
#define PTR_ORG 0
#define WORDALIGNMENT 8
// Possibly OptionalWordAlign or other macros
#endif
```

**Verification:**
Create a summary document: `docs/attempts/ATTEMPT-01/alpha-config-extracted.md`

**Success Criteria:**
- All Alpha #ifdef blocks documented
- TADBITS, PTR_ORG, WORDALIGNMENT values confirmed
- Any conditional compilation differences noted

---

### Step 1.3: Extract Alpha Register Definitions

**Action:** Document Alpha HARDREGS configuration.

**Command:**
```bash
grep -A 30 "alpha\|ALPHA" emulator/regdefs.h
```

**What to Extract:**
- Which registers are used (Alpha has ~10 registers)
- Register names (e.g., `$9`, `$10`, `$11`, etc.)
- What each register is assigned to (`exs`, `pc`, `op`, `areg`, `yreg`, `andb`, etc.)

**Expected Pattern:**
```c
#ifdef __alpha
#define Register register
register exstate *exs asm("$9");
register code *pc asm("$10");
register opcode op asm("$11");
// ... etc
#endif
```

**Create Document:** `docs/attempts/ATTEMPT-01/alpha-register-mapping.md`

**Table Format:**
| VM Variable | Alpha Register | Purpose |
|-------------|----------------|---------|
| exs         | $9             | Execution state |
| pc          | $10            | Program counter |
| ...         | ...            | ... |

**Success Criteria:**
- All Alpha register assignments documented
- Count matches expected (~10 registers)
- Purpose of each register understood

---

### Step 1.4: Find Alpha-Specific Build Logic

**Action:** Document build system differences for Alpha.

**Command:**
```bash
grep -n -A 5 -B 5 "alpha" configure.in
```

**What to Look For:**
- Host detection patterns
- Compiler flags
- Library dependencies
- Linker options

**Verification:**
Create section in `ATTEMPT-01-LOG.md` with all Alpha build logic.

**Success Criteria:**
- All configure.in Alpha references documented
- Compiler flags noted
- Any special build requirements identified

---

### Step 1.5: Search for Runtime Alpha Differences

**Action:** Check if any runtime code has Alpha-specific paths.

**Command:**
```bash
grep -rn "alpha\|ALPHA\|__alpha" emulator/*.c
```

**Expected:** Likely none (or minimal) - most differences should be in headers/config.

**Verification:**
Document findings in `ATTEMPT-01-LOG.md`.

**Success Criteria:**
- All runtime differences documented
- If none found, explicitly state "No runtime Alpha-specific code found"

---

## 4. Phase 2: Pattern Extraction

**Goal:** Create a systematic mapping guide from Alpha patterns to x86-64.

### Step 2.1: Create Porting Pattern Document

**Action:** Create comprehensive mapping document.

**File:** `docs/attempts/ATTEMPT-01/alpha-to-x86-64-mapping.md`

**Required Sections:**

#### 2.1.1 Platform Macros Mapping

| Alpha | x86-64 | Rationale |
|-------|--------|-----------|
| `#ifdef __alpha` | `#if defined(__x86_64__) \|\| defined(__amd64__)` | GCC standard macros |
| TADBITS 64 | TADBITS 64 | Same (64-bit) |
| PTR_ORG 0 | PTR_ORG 0 | Same (Alpha proved this works) |
| WORDALIGNMENT 8 | WORDALIGNMENT 8 | Same (8-byte alignment) |

#### 2.1.2 Register Mapping

| VM Variable | Alpha Reg | x86-64 Reg | Notes |
|-------------|-----------|------------|-------|
| exs         | $9        | r15        | Callee-saved |
| pc          | $10       | r14        | Callee-saved |
| op          | $11       | r13        | Callee-saved |
| areg        | $12       | r12        | Callee-saved |
| yreg        | $13       | rbx        | Callee-saved |
| andb        | $14       | rbp        | Callee-saved (use with care) |

**Why these registers?**
- r12-r15: Callee-saved in x86-64 ABI, perfect for hot variables
- rbx: Traditional callee-saved register
- rbp: Callee-saved, but normally used for frame pointer (OK without -fno-omit-frame-pointer)

#### 2.1.3 ASM Syntax Mapping

| Alpha GCC | x86-64 GCC |
|-----------|------------|
| `register type var asm("$9");` | `register type var asm("r15");` |
| `register type var asm("$10");` | `register type var asm("r14");` |

**Verification:**
Review document for completeness - all Alpha patterns must have x86-64 equivalent.

**Success Criteria:**
- All mappings documented
- Rationale provided for each choice
- No ambiguous mappings

---

### Step 2.2: Create Verification Scripts

**Goal:** Prepare scripts to verify each step.

**File:** `scripts/verify-platform-config.sh`

```bash
#!/bin/bash
# Verify platform configuration is correct

echo "=== Platform Configuration Verification ==="

# Check for x86-64 definition
if grep -q "x86_64\|amd64" emulator/sysdeps.h; then
    echo "✓ x86-64 detection present"
else
    echo "✗ x86-64 detection missing"
    exit 1
fi

# Check TADBITS
if grep -A 3 "x86_64\|amd64" emulator/sysdeps.h | grep -q "TADBITS.*64"; then
    echo "✓ TADBITS = 64"
else
    echo "✗ TADBITS not set correctly"
    exit 1
fi

# Check PTR_ORG
if grep -A 5 "x86_64\|amd64" emulator/sysdeps.h | grep -q "PTR_ORG.*0"; then
    echo "✓ PTR_ORG = 0"
else
    echo "✗ PTR_ORG not set correctly"
    exit 1
fi

# Check WORDALIGNMENT
if grep -A 5 "x86_64\|amd64" emulator/sysdeps.h | grep -q "WORDALIGNMENT.*8"; then
    echo "✓ WORDALIGNMENT = 8"
else
    echo "✗ WORDALIGNMENT not set correctly"
    exit 1
fi

echo "=== All checks passed ==="
```

**File:** `scripts/verify-register-defs.sh`

```bash
#!/bin/bash
# Verify register definitions for x86-64

echo "=== Register Definitions Verification ==="

REGFILE="emulator/regdefs.h"

# Check for x86-64 section
if grep -q "x86_64\|amd64" "$REGFILE"; then
    echo "✓ x86-64 HARDREGS section present"
else
    echo "✗ x86-64 HARDREGS section missing"
    exit 1
fi

# Check for key registers
for reg in r15 r14 r13 r12 rbx rbp; do
    if grep -A 20 "x86_64\|amd64" "$REGFILE" | grep -q "\"$reg\""; then
        echo "✓ Register $reg defined"
    else
        echo "✗ Register $reg missing"
        exit 1
    fi
done

echo "=== All checks passed ==="
```

**Make Executable:**
```bash
chmod +x scripts/verify-platform-config.sh
chmod +x scripts/verify-register-defs.sh
```

**Success Criteria:**
- Scripts created
- Scripts are executable
- Scripts test all critical configuration

---

## 5. Phase 3: x86-64 Configuration

**Goal:** Add x86-64 platform detection and configuration.

### Step 3.1: Add x86-64 Platform Detection to sysdeps.h

**Action:** Add x86-64 configuration block to `emulator/sysdeps.h`.

**Location:** Find the Alpha block and add x86-64 block nearby.

**Code to Add:**
```c
#if defined(__x86_64__) || defined(__amd64__)
#define TADBITS 64
#define PTR_ORG 0
#define WORDALIGNMENT 8
#endif
```

**Exact Steps:**

1. Read `emulator/sysdeps.h`
2. Find the Alpha block (search for `#ifdef __alpha`)
3. Add x86-64 block immediately after Alpha block
4. Keep formatting consistent with existing code

**Verification:**
```bash
bash scripts/verify-platform-config.sh
```

**Success Criteria:**
- Script passes all checks
- x86-64 config matches Alpha config exactly (TADBITS=64, PTR_ORG=0, WORDALIGNMENT=8)
- No syntax errors (check with: `gcc -E emulator/sysdeps.h -o /tmp/sysdeps.i`)

**Document:** Add to `ATTEMPT-01-LOG.md` with verification output.

---

### Step 3.2: Add x86-64 Register Definitions to regdefs.h

**Action:** Add HARDREGS configuration for x86-64.

**File:** `emulator/regdefs.h`

**Code to Add:**
```c
#if defined(__x86_64__) || defined(__amd64__)
#ifdef HARDREGS
#define Register register
register exstate *exs asm("r15");
register code *pc asm("r14");
register opcode op asm("r13");
register Term *areg asm("r12");
register Term *yreg asm("rbx");
register andbox *andb asm("rbp");
register Term tmpreg asm("r10");  // Scratch register
register Integer intreg asm("r11d");  // 32-bit scratch
#else
#define Register
#endif
#endif /* x86_64 */
```

**Exact Steps:**

1. Read `emulator/regdefs.h`
2. Find the Alpha HARDREGS block
3. Add x86-64 block after Alpha
4. Use same structure as Alpha

**Verification:**
```bash
bash scripts/verify-register-defs.sh
```

**Alternative Verification:**
```bash
# Check that registers are recognized
echo 'register int x asm("r15");' | gcc -c -x c - -o /tmp/test.o 2>&1
# Should succeed with no errors
```

**Success Criteria:**
- Script passes
- GCC accepts the register constraints
- Same number of registers as Alpha (~8-10 registers)

---

### Step 3.3: Update configure.in for x86-64

**Action:** Add x86-64 to host detection in configure.in.

**What to Change:**

Find host detection case statement (likely looks like):
```
case "$host" in
  sparc-*-*)
    # SPARC config
    ;;
  alpha-*-*)
    # Alpha config
    ;;
  ...
esac
```

Add x86-64 case:
```
  x86_64-*-* | amd64-*-*)
    # x86-64 (64-bit Intel/AMD)
    CC="${CC-gcc}"
    CFLAGS="${CFLAGS--O2 -Wall}"
    ;;
```

**Verification:**
```bash
# Check syntax
autoconf --version  # See what version we have
grep -A 3 "x86_64" configure.in
```

**Success Criteria:**
- x86-64 case added
- Syntax matches existing cases
- No special flags needed (rely on defaults for now)

**Note:** configure.in is ancient (autoconf 1.x), so keep changes minimal.

---

### Step 3.4: Replace Deprecated Functions

**Goal:** Fix functions that won't compile on modern systems.

#### 3.4.1: Replace getwd() with getcwd()

**Files to Check:**
```bash
grep -rn "getwd" emulator/
```

**For Each Occurrence:**

Old code:
```c
char *path = getwd(buffer);
```

New code:
```c
char *path = getcwd(buffer, sizeof(buffer));
```

**Verification:**
```bash
# Should find no getwd
grep -r "getwd" emulator/
```

#### 3.4.2: Replace gcvt() with snprintf()

**Files to Check:**
```bash
grep -rn "gcvt" emulator/
```

**For Each Occurrence:**

Old code:
```c
gcvt(value, ndigits, buffer);
```

New code:
```c
snprintf(buffer, sizeof(buffer), "%.*g", ndigits, value);
```

**Verification:**
```bash
# Should find no gcvt
grep -r "gcvt" emulator/
```

**Success Criteria:**
- All deprecated functions replaced
- grep finds no remaining occurrences
- Replacements are functionally equivalent

---

## 6. Phase 4: Incremental Implementation

**Goal:** Build the system incrementally, fixing issues as they arise.

### Step 4.1: Generate configure Script

**Action:**
```bash
cd /home/user/akl-agents
autoconf
```

**Expected Issues:**
- May warn about ancient autoconf version
- May fail if autoconf 1.x not available

**Fallback:** If autoconf fails, manually edit the `configure` script (it's just a shell script).

**Verification:**
```bash
# Check configure was updated
ls -l configure
# Check it has x86-64 detection
grep "x86_64\|amd64" configure
```

**Success Criteria:**
- configure script exists and is executable
- Contains x86-64 detection logic

---

### Step 4.2: Run Configure for x86-64

**Action:**
```bash
./configure
```

**Expected Output:**
- Should detect x86-64 host
- Should set up Makefiles
- May produce warnings (OK for now)

**Verification:**
```bash
# Check that config.h was created
ls -l config.h
# Check detected platform
grep "x86_64\|amd64\|TADBITS" config.h
```

**Success Criteria:**
- configure completes (warnings OK, errors NOT OK)
- config.h created
- Platform correctly detected

**Document:** Save full configure output to `ATTEMPT-01-LOG.md`.

---

### Step 4.3: Attempt First Build (Expect Failures)

**Action:**
```bash
make 2>&1 | tee build.log
```

**Expected Outcome:** Build will likely fail. This is normal and expected.

**What to Capture:**
- First error encountered
- Which file failed to compile
- Error message

**Analysis:**

Common first-time errors:
1. **Missing headers** → Add includes
2. **Incompatible types** → Check TADBITS propagation
3. **Register constraints invalid** → Check GCC version/compatibility
4. **Undefined references** → Check linking

**Verification:**
```bash
# Count errors
grep -c "error:" build.log

# Get first error
grep "error:" build.log | head -1
```

**Success Criteria (for this step):**
- Build attempted
- First error identified
- Error documented in ATTEMPT-01-LOG.md
- Root cause hypothesized

**DO NOT PROCEED** until first error is resolved or escalated.

---

### Step 4.4: Fix First Error

**Action:** Based on the error in Step 4.3, apply appropriate fix.

**Process:**
1. Read the file that failed
2. Understand the error
3. Apply minimal fix
4. Rebuild
5. Document the fix

**Example Fixes:**

**Error Type 1: Missing include**
```
emulator/file.c:123: error: implicit declaration of function 'getcwd'
```
**Fix:** Add `#include <unistd.h>` to file.c

**Error Type 2: Type mismatch**
```
emulator/term.c:456: error: incompatible types in assignment
```
**Fix:** Check if pointer size assumptions (32-bit vs 64-bit) are the issue.

**Error Type 3: Register constraint**
```
emulator/engine.c:789: error: impossible register constraint
```
**Fix:** Register might not be available on x86-64, choose alternative.

**Verification:**
```bash
# Rebuild just the failed file
make emulator/file.o
```

**Success Criteria:**
- Error fixed
- File now compiles
- Fix documented

---

### Step 4.5: Iterative Build-Fix Cycle

**Process:**

**Repeat until build succeeds:**
1. Run `make 2>&1 | tee -a build.log`
2. Identify next error
3. Document error in log
4. Apply fix
5. Verify fix compiles
6. Continue

**Track Progress:**
```bash
# After each successful fix
echo "$(date): Fixed error in <file>" >> ATTEMPT-01-LOG.md
```

**Stopping Conditions:**

**STOP and escalate if:**
- Same error appears >3 times after different fixes
- Error is in build system, not source code
- Error suggests fundamental architecture incompatibility
- Stuck for >30 minutes on single error

**Success Criteria:**
- All compilation errors resolved
- `make` completes successfully
- Executables produced

**Expected Time:** 5-20 iterations, depending on issues.

---

## 7. Phase 5: Build and Test

**Goal:** Verify the build is functional and passes tests.

### Step 7.1: Verify Build Artifacts

**Action:**
```bash
ls -lh emulator/agents emulator/emulator
file emulator/agents emulator/emulator
```

**Expected:**
- `emulator/agents` - main executable
- `emulator/emulator` - possibly another binary or symlink
- Should be x86-64 ELF binaries

**Verification:**
```bash
# Check architecture
file emulator/agents | grep "x86-64\|x86_64"
```

**Success Criteria:**
- Executables exist
- Are x86-64 binaries
- Are executable (`chmod +x` if needed)

---

### Step 7.2: Basic Smoke Test

**Action:** Try running the emulator with minimal input.

```bash
echo "test" | emulator/agents
```

**Expected Outcomes:**

**Best case:** Runs without crashing, produces output
**Acceptable:** Crashes gracefully with error message
**Bad:** Segfaults immediately

**Verification:**
```bash
echo $?  # Check exit code
```

**Analysis:**
- Exit code 0 = success
- Exit code 1-127 = controlled exit
- Exit code 139 = segfault (128 + SIGSEGV(11))

**Document:** Output and behavior in ATTEMPT-01-LOG.md.

---

### Step 7.3: Run DejaGnu Test Suite

**Action:**
```bash
cd tests
make check 2>&1 | tee test-results.log
```

**Alternative (if make check not available):**
```bash
runtest --tool agents --srcdir tests
```

**Expected:** ~60 tests across 9 categories.

**Capture:**
- Number of tests passed
- Number of tests failed
- Which tests failed

**Verification:**
```bash
# Parse results
grep "# of expected passes" test-results.log
grep "# of unexpected failures" test-results.log
grep "# of unresolved testcases" test-results.log
```

**Success Criteria:**
- Tests run (even if many fail)
- Pass rate documented
- Failed tests categorized

**Target:** >95% pass rate for "success", but anything >50% is encouraging for first attempt.

---

### Step 7.4: Analyze Test Failures

**Action:** For each failed test, determine category.

**Failure Categories:**

1. **Unrelated to porting** (pre-existing failures)
2. **Endianness issues** (little vs big endian)
3. **Pointer size issues** (32-bit assumptions)
4. **Register allocation issues** (HARDREGS problems)
5. **Memory alignment issues**
6. **Instruction dispatch issues**

**Process:**

For top 5 failures:
1. Read test source in `tests/`
2. Run test individually with verbose output
3. Determine failure mode
4. Categorize

**Document:** Create table in ATTEMPT-01-LOG.md:

| Test Name | Category | Error | Hypothesis | Priority |
|-----------|----------|-------|------------|----------|
| foo.test  | Pointer size | Assertion failed | 32-bit assumption | High |
| bar.test  | Endianness | Wrong output | Byte order | Medium |

**Success Criteria:**
- All failures categorized
- Top 5 analyzed in detail
- Priorities assigned

---

### Step 7.5: Fix Critical Test Failures

**Action:** Fix highest-priority failures first.

**Process:**
1. Start with "High" priority
2. Apply fix
3. Rerun specific test
4. Verify fix doesn't break other tests
5. Document fix

**Example:**

**Failure:** `memory.test` fails with alignment error
**Fix:** Add OptionalWordAlign to x86-64 config
**Verify:** Rerun `memory.test`

**Stop Condition:** After 5 fixes OR 2 hours, whichever comes first.

**Success Criteria:**
- At least 3 high-priority failures fixed
- Pass rate improved
- No regressions introduced

---

## 8. Phase 6: ARM64 Adaptation

**Goal:** Port to ARM64 using lessons from x86-64.

**Note:** Only start this phase after x86-64 build is stable (>90% test pass rate).

### Step 8.1: Create ARM64 Configuration

**Action:** Add ARM64 block to `emulator/sysdeps.h`.

**Code:**
```c
#if defined(__aarch64__) || defined(__arm64__)
#define TADBITS 64
#define PTR_ORG 0
#define WORDALIGNMENT 8
// May need OptionalWordAlign - TBD based on testing
#endif
```

---

### Step 8.2: ARM64 Register Mapping

**Action:** Add HARDREGS for ARM64 to `emulator/regdefs.h`.

**Register Choices (ARM64 ABI):**
- x19-x28: Callee-saved registers (perfect for HARDREGS)
- x9-x15: Temporary registers (for scratch)

**Code:**
```c
#if defined(__aarch64__) || defined(__arm64__)
#ifdef HARDREGS
#define Register register
register exstate *exs asm("x19");
register code *pc asm("x20");
register opcode op asm("x21");
register Term *areg asm("x22");
register Term *yreg asm("x23");
register andbox *andb asm("x24");
register Term tmpreg asm("x25");
register Integer intreg asm("w26");  // 32-bit version
#else
#define Register
#endif
#endif
```

---

### Step 8.3: ARM64 Build

**Action:** Add ARM64 case to configure.in, rebuild.

**Follow same process as x86-64:**
1. Update configure.in
2. Run autoconf
3. Run configure
4. Run make
5. Fix errors incrementally
6. Run tests

**Leverage Knowledge:** Use fixes from x86-64 experience to anticipate ARM64 issues.

---

## 9. Verification Commands

Quick reference of all verification commands:

```bash
# Platform detection
bash scripts/verify-platform-config.sh

# Register definitions
bash scripts/verify-register-defs.sh

# Build verification
make clean && make 2>&1 | tee build.log
grep -c "error:" build.log

# Binary verification
file emulator/agents

# Test suite
cd tests && make check 2>&1 | tee test-results.log

# Test summary
grep "# of expected passes" test-results.log
grep "# of unexpected failures" test-results.log
```

---

## 10. Rollback Procedures

### 10.1 Rollback Last Change

```bash
git diff HEAD  # Review changes
git checkout HEAD -- <file>  # Rollback specific file
```

### 10.2 Rollback to Known-Good State

```bash
git log --oneline  # Find last good commit
git reset --hard <commit-hash>
```

### 10.3 Branch Restart

```bash
# If attempt is completely broken
git checkout claude/porting-study-011CUoHSMTFawkiKfVsuVuPP
git branch -D claude/port-attempt-01
git checkout -b claude/port-attempt-02  # Start fresh
```

---

## Summary

This plan provides:
- **67 explicit steps** across 6 phases
- **Verification commands** for every critical change
- **Expected failure modes** and how to handle them
- **Rollback procedures** if things go wrong
- **Stop conditions** to avoid infinite loops

**Remember:**
- Document everything in ATTEMPT-XX-LOG.md
- One step at a time
- Verify before proceeding
- It's OK to fail - we learn and retry

**Next Steps:**
- Create Document 13: Iterative Strategy
- Set up attempt infrastructure
- Create new branch
- Begin Phase 1

---

**Related Documents:**
- **11-CLAUDE-CODE-REALITY-CHECK.md** - Why Option B was chosen
- **13-ITERATIVE-STRATEGY.md** - How we learn from failures
- **09-IMPLEMENTATION-ROADMAP.md** - High-level roadmap (both options)
