# AGENTS v0.9 - Claude Code Implementation Roadmap

**Document:** Phase 5, Document 9 of 10
**Status:** Complete
**Date:** 2025-11-05
**Purpose:** Staged implementation plan for Claude Code autonomous porting

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Claude Code Capabilities and Constraints](#claude-code-capabilities-and-constraints)
3. [Chosen Strategy: Staged Vintage Emulation](#chosen-strategy-staged-vintage-emulation)
4. [Phase 0: Vintage Environment Establishment](#phase-0-vintage-environment-establishment)
5. [Phase 1: Vintage Bootstrap Verification](#phase-1-vintage-bootstrap-verification)
6. [Phase 2: Incremental Modernization](#phase-2-incremental-modernization)
7. [Phase 3: Architecture Transition](#phase-3-architecture-transition)
8. [Phase 4: Modern Platform Validation](#phase-4-modern-platform-validation)
9. [Verification Strategy](#verification-strategy)
10. [Session Management](#session-management)
11. [Rollback and Recovery](#rollback-and-recovery)
12. [Critical Feasibility Analysis](#critical-feasibility-analysis)
13. [References](#references)

---

## Executive Summary

This roadmap outlines a **staged porting strategy** to be executed by **Claude Code** (autonomous AI coding agent), not a human development team.

### Core Strategy: Vintage Emulation Path

**Rationale from Document 06:** The AGENTS build system is 30+ years old. Attempting to build directly on modern systems risks catastrophic failures with no baseline for comparison.

**Approach:** Establish working system in vintage environment first, then incrementally modernize.

### Implementation Stages

```
Stage 0: Vintage Setup          → QEMU + Solaris 2.5.1 / NetBSD 1.x
         ↓
Stage 1: Vintage Verification   → Build original system, verify tests pass
         ↓
Stage 2: Incremental Modern     → Update OS → gcc 4.x → gcc 8 → gcc 11
         ↓
Stage 3: Architecture Pivot     → SPARC 32→64 → x86-64 → ARM64
         ↓
Stage 4: Modern Validation      → Native modern Linux builds
```

### Claude Code Execution Model

**NOT a human team:**
- No "280-560 hours" estimates (meaningless for AI)
- No "hire a developer" (I am the developer)
- No "consult expert" (must work from documentation)

**Session-based execution:**
- Work in discrete verification checkpoints
- Each checkpoint: change → verify → commit
- Session timeout: plan for 2-10 minute build windows
- State persistence: everything written to files/git

### Critical Feasibility Warning

⚠️ **See Section 12** for honest assessment of what Claude Code can and cannot do. Some steps in this roadmap may require **human intervention** or **alternative approaches**.

---

## Claude Code Capabilities and Constraints

### What I CAN Do ✅

**Code Analysis:**
- Read and understand complex C codebases
- Search for patterns (Glob, Grep)
- Static analysis of dependencies
- Cross-reference tracking

**Code Modification:**
- Edit files with surgical precision
- Write new files
- Refactor code systematically
- Apply transformations across multiple files

**Build Automation:**
- Run shell commands (bash)
- Execute compilers (gcc, clang)
- Run configure scripts
- Execute make (with timeout constraints)

**Verification:**
- Run test suites (DejaGnu)
- Compare outputs
- Validate file contents
- Check compilation success

**Documentation:**
- Create comprehensive documentation
- Track decisions and rationale
- Maintain change logs

### What I CANNOT Do ❌

**Interactive Operations:**
- Cannot handle interactive prompts easily
- Cannot use GUI applications
- Cannot manually debug in gdb interactively
- Cannot respond to "Press Y to continue"

**Long-Running Processes:**
- Maximum command timeout: 10 minutes
- Full AGENTS build may take 30+ minutes
- Cannot wait for multi-hour operations

**Emulator Interaction:**
- Cannot easily log into QEMU console interactively
- Cannot navigate through boot menus
- Cannot handle graphical emulator interfaces

**Cross-Session State:**
- No memory between sessions (unless persisted to files)
- Must reconstruct context from files/git each session
- Cannot "remember" debugging insights

**External Resources:**
- Cannot download arbitrary files (no WebFetch for binaries)
- Cannot access restricted networks
- Limited to what's available via bash

### Implications for Strategy

**Must adapt Vintage Emulation Strategy:**
1. **Automate emulator interaction** (expect/pexpect scripts)
2. **Use SSH into QEMU** (not console)
3. **Break builds into chunks** (< 10 min each)
4. **Heavy reliance on scripts** (not manual intervention)
5. **Extensive pre-analysis** (understand before executing)

---

## Chosen Strategy: Staged Vintage Emulation

From **Document 06 Section 9.3**, adapted for Claude Code execution.

### Why This Approach?

**Problem:** 30-year-old build system with:
- Deprecated functions (getwd, gcvt)
- Ancient autoconf (pre-2.0)
- Unknown build dependencies
- Untested on modern systems

**Solution:** Establish **known-good baseline** in vintage environment, then modernize incrementally.

### Stage Evolution Path

| Stage | Platform | OS | Compiler | Purpose |
|-------|----------|----|---------|---------|
| **0** | QEMU SPARC | Solaris 2.5.1 | gcc 2.7.x | Setup vintage environment |
| **1** | QEMU SPARC | Solaris 2.5.1 | gcc 2.7.x | Verify original builds |
| **2a** | QEMU SPARC | Solaris 9 | gcc 4.x | Modernize OS |
| **2b** | QEMU SPARC | NetBSD 9 | gcc 8.x | Modern POSIX |
| **2c** | QEMU SPARC64 | NetBSD 9 | gcc 10.x | 64-bit on SPARC |
| **3a** | Native x86-64 | Ubuntu 22.04 | gcc 11 | Architecture pivot |
| **3b** | Native ARM64 | Ubuntu 22.04 | gcc 11 | Second architecture |
| **4** | Both native | Ubuntu 22.04 | gcc 11/clang 14 | Production validation |

### Verification at Each Stage

Every stage must achieve:
1. ✅ **Build success**: `make all` completes without errors
2. ✅ **Test passage**: DejaGnu tests pass (>95%)
3. ✅ **Runtime verification**: Execute simple programs
4. ✅ **Comparison**: Output matches previous stage (if applicable)

---

## Phase 0: Vintage Environment Establishment

**Goal:** Set up QEMU with period-appropriate OS and toolchain

**Duration:** 1-3 sessions (environment setup is tricky)

### Task 0.1: QEMU Installation

**Prerequisites:**
- Modern Linux host (already have: Ubuntu/Debian in Claude Code environment)
- QEMU package available

**Actions:**
```bash
# Install QEMU with SPARC support
sudo apt-get update
sudo apt-get install -y qemu-system-sparc qemu-utils

# Verify installation
qemu-system-sparc --version
```

**Verification:**
- QEMU binary exists and runs
- SPARC emulation available

**Claude Code Feasibility:** ✅ **HIGH** - straightforward package installation

---

### Task 0.2: Obtain Vintage OS Image

**Options:**
1. **Solaris 2.5.1** (Sun Microsystems, 1996)
   - Authentic vintage environment
   - Requires ISO and license consideration

2. **NetBSD 1.6** (Open source, 2002)
   - Freely available
   - Good SPARC support
   - More accessible

**Actions:**
```bash
# Download NetBSD 1.6 SPARC ISO (public archive)
wget http://archive.netbsd.org/pub/NetBSD-archive/NetBSD-1.6/sparc/installation/netboot/netbsd.ram.gz

# Or use pre-configured image if available
# Note: May need to use existing archived images
```

**Verification:**
- ISO/image file downloaded
- Checksum matches (if available)

**Claude Code Feasibility:** ⚠️ **MEDIUM** - depends on image availability and licensing

**Alternative:**  Skip vintage, start with NetBSD 9 (modern but still SPARC-supporting)

---

### Task 0.3: QEMU Configuration

**Actions:**
Create QEMU launch script:

```bash
#!/bin/bash
# qemu-sparc-netbsd.sh

qemu-system-sparc \
  -M SS-5 \
  -m 256 \
  -hda netbsd-sparc.img \
  -cdrom netbsd-1.6-sparc.iso \
  -boot d \
  -nographic \
  -serial mon:stdio
```

**Verification:**
- QEMU starts without errors
- Can reach bootloader prompt

**Claude Code Feasibility:** ⚠️ **MEDIUM** - interactive boot may be challenging

---

### Task 0.4: OS Installation Automation

**Problem:** OS installation is typically interactive

**Solutions:**
1. **Pre-built image:** Find existing NetBSD SPARC image with gcc pre-installed
2. **Automated installation:** Use expect scripts to automate installer
3. **Skip to modern:** Use NetBSD 9 (easier setup, still SPARC)

**Example expect script:**
```tcl
#!/usr/bin/expect -f
spawn qemu-system-sparc -M SS-5 -m 256 -hda netbsd.img -cdrom netbsd.iso -nographic
expect "boot:" { send "install\r" }
expect "Terminal type?" { send "vt100\r" }
# ... continue automation ...
```

**Verification:**
- OS installs successfully
- Can log in
- gcc is available

**Claude Code Feasibility:** ⚠️ **LOW-MEDIUM** - complex interactive automation

**Recommended:** Use pre-built image or skip to modern NetBSD 9

---

### Task 0.5: Network Configuration for SSH

**Goal:** Enable SSH access to QEMU guest (easier than console)

**Actions:**
```bash
# QEMU with user-mode networking and port forwarding
qemu-system-sparc \
  -M SS-5 \
  -m 256 \
  -hda netbsd-sparc.img \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -device ne2k_isa,netdev=net0 \
  -nographic \
  -daemonize

# Then connect via SSH
ssh -p 2222 root@localhost
```

**Verification:**
- SSH connection succeeds
- Can execute commands remotely
- Can transfer files (scp)

**Claude Code Feasibility:** ✅ **HIGH** - SSH automation is much easier than console

**This is CRITICAL for Claude Code execution!**

---

### Phase 0 Exit Criteria

✅ QEMU environment is operational
✅ Vintage OS is installed and accessible
✅ SSH access configured and working
✅ gcc and basic build tools available
✅ Can transfer AGENTS source code into environment

**Reality Check:** Phase 0 may require **human assistance** for initial setup, then Claude Code can take over for actual porting work.

---

## Phase 1: Vintage Bootstrap Verification

**Goal:** Prove that AGENTS builds and runs in vintage environment

**Duration:** 2-5 sessions

### Task 1.1: Transfer Source Code

**Actions:**
```bash
# From host to QEMU guest
scp -P 2222 -r /path/to/agents root@localhost:/root/agents

# Verify transfer
ssh -p 2222 root@localhost "ls -la /root/agents"
```

**Verification:**
- All source files present
- Directory structure intact
- No corruption

**Claude Code Feasibility:** ✅ **HIGH**

---

### Task 1.2: Run Configure

**Actions:**
```bash
ssh -p 2222 root@localhost "cd /root/agents && ./configure"
```

**Expected:** Configure completes, creates Makefile

**Possible Issues:**
- autoconf version mismatch
- Missing dependencies (m4, etc.)
- Platform not recognized

**Claude Code Actions:**
- Capture configure output
- Parse for errors
- If platform unknown, may need to update config.guess

**Verification:**
- Configure succeeds (exit code 0)
- Makefile generated
- config.status created

**Claude Code Feasibility:** ✅ **HIGH** - can run and parse output

---

### Task 1.3: Initial Build Attempt

**Actions:**
```bash
ssh -p 2222 root@localhost "cd /root/agents && make all 2>&1 | tee build.log"
```

**Expected:** Either success or well-defined failures

**Timeout Concern:** Full build may take 20-30 minutes in QEMU

**Solutions:**
1. **Increase timeout:** Use 600000ms (10 min) - may not be enough
2. **Build in background:**
   ```bash
   ssh -p 2222 root@localhost "cd /root/agents && nohup make all > build.log 2>&1 &"
   # Check later
   ssh -p 2222 root@localhost "tail -f /root/agents/build.log"
   ```
3. **Staged builds:**
   ```bash
   make -C emulator  # Build emulator only first
   make -C compiler  # Then compiler
   # etc.
   ```

**Verification:**
- Build completes (check exit code or log)
- Executables generated (agents, oagents)
- No fatal errors in log

**Claude Code Feasibility:** ⚠️ **MEDIUM** - timeout management required

---

### Task 1.4: Run Test Suite

**Actions:**
```bash
ssh -p 2222 root@localhost "cd /root/agents/tests && runtest --tool agents AGENTS=../agents"
```

**Expected:** Tests run, some pass

**Verification:**
- DejaGnu runs
- agents.sum generated
- Count passes/fails

**Claude Code Feasibility:** ✅ **HIGH** - test execution straightforward

---

### Task 1.5: Baseline Documentation

**Actions:**
- Save build log
- Save test results
- Document exact environment (OS version, gcc version, etc.)
- Create snapshot: `build-baseline-sparc-vintage.tar.gz`

**Verification:**
- All artifacts saved to git
- Reproducible environment documented

**Claude Code Feasibility:** ✅ **HIGH** - documentation is my strength

---

### Phase 1 Exit Criteria

✅ AGENTS builds successfully in vintage environment
✅ Test suite runs (even if some tests fail)
✅ Baseline documented and preserved
✅ Known-good configuration archived

**If Phase 1 fails:** The system may need repairs before modernization. This is valuable information!

---

## Phase 2: Incremental Modernization

**Goal:** Step through OS/compiler upgrades while staying on SPARC

**Duration:** 5-10 sessions

### Stage 2a: Solaris 9 / gcc 4.x

**Actions:**
1. Set up QEMU with Solaris 9 or NetBSD 5.x
2. Install gcc 4.x
3. Attempt build with minimal changes
4. Fix incompatibilities (deprecated functions, etc.)
5. Verify tests still pass

**Expected Changes:**
- Replace `getwd()` with `getcwd()`
- Replace `gcvt()` with `snprintf()`
- Update function signatures for prototypes

**Verification:**
- Build succeeds
- Tests pass with similar results to Stage 1

**Claude Code Feasibility:** ✅ **HIGH** - code changes are straightforward

---

### Stage 2b: NetBSD 9 / gcc 8.x

**Actions:**
1. Further compiler modernization
2. Enable more warnings (-Wall -Wextra)
3. Fix warnings
4. Update autoconf if needed

**Verification:**
- Clean compilation
- All tests pass
- No deprecation warnings

**Claude Code Feasibility:** ✅ **HIGH**

---

### Stage 2c: SPARC 64-bit

**Actions:**
1. Set up SPARC64 environment (or use NetBSD's 64-bit mode)
2. Change TADBITS=64 in configuration
3. Build and test
4. Compare behavior to Alpha 64-bit port

**This proves 32→64 bit transition before architecture change**

**Verification:**
- 64-bit build succeeds
- Tests pass
- Matches Alpha behavior

**Claude Code Feasibility:** ✅ **HIGH** - follows documented Alpha path

---

### Phase 2 Exit Criteria

✅ Modern compiler (gcc 8+) supported
✅ Code compiles cleanly with warnings enabled
✅ 64-bit transition successful on SPARC
✅ Test passage maintained throughout

---

## Phase 3: Architecture Transition

**Goal:** Move from SPARC to x86-64 and ARM64

**Duration:** 5-15 sessions (complex)

### Task 3.1: Update Platform Detection

**Files:** `emulator/sysdeps.h`, `emulator/regdefs.h`, `configure.in`

**Actions:**
```c
// sysdeps.h
#if defined(__x86_64__) || defined(__amd64__)
#define TADBITS 64
#define WORDALIGNMENT 8
#endif

#if defined(__aarch64__) || defined(__arm64__)
#define TADBITS 64
#define WORDALIGNMENT 8
// May need OptionalWordAlign
#endif
```

**Claude Code Feasibility:** ✅ **HIGH** - straightforward code additions

---

### Task 3.2: Cross-Compilation Test

**Actions:**
```bash
# Install cross-compilers on host
sudo apt-get install gcc-aarch64-linux-gnu

# Try cross-compile
./configure --host=aarch64-linux-gnu
make
```

**Verification:**
- Cross-compilation succeeds
- Binaries are ARM64 (file command)

**Cannot run tests yet** (wrong architecture)

**Claude Code Feasibility:** ✅ **HIGH**

---

### Task 3.3: Native x86-64 Build

**Actions:**
```bash
# On native x86-64 host (Claude Code environment)
./configure
make all
```

**Expected:** Should work if Stage 2c (SPARC64) worked

**Verification:**
- Build succeeds
- Emulator runs
- Tests pass

**Claude Code Feasibility:** ✅ **HIGH** - this is my native environment!

---

### Task 3.4: Native ARM64 Build

**Options:**
1. **QEMU ARM64:** Similar to SPARC approach
2. **Cloud ARM64 instance:** If available
3. **Cross-compile only:** Defer native testing

**Claude Code Feasibility:** ⚠️ **MEDIUM** - depends on environment availability

---

### Task 3.5: Hard Register Allocation

**Actions:**
- Define HARDREGS for x86-64 and ARM64 (from Document 03)
- Build with -DHARDREGS
- Benchmark performance improvement

**Verification:**
- Performance increases 10-20%
- Tests still pass
- No register allocation conflicts

**Claude Code Feasibility:** ✅ **HIGH**

---

### Phase 3 Exit Criteria

✅ x86-64 native build successful
✅ ARM64 build successful (native or cross)
✅ Tests pass on both architectures
✅ Performance acceptable (with HARDREGS)
✅ Both platforms show behavioral parity

---

## Phase 4: Modern Platform Validation

**Goal:** Final validation on modern systems

**Duration:** 3-5 sessions

### Task 4.1: Modern Toolchain Testing

**Actions:**
```bash
# Test with latest compilers
export CC=gcc-12
./configure && make clean && make all && make check

export CC=clang-16
./configure && make clean && make all && make check
```

**Verification:**
- Both gcc and clang work
- No compiler-specific issues

**Claude Code Feasibility:** ✅ **HIGH**

---

### Task 4.2: Full Test Suite

**Actions:**
- Run complete DejaGnu suite
- Run all demos
- Stress test (long-running programs)

**Verification:**
- >95% test passage
- All demos execute correctly
- No crashes or memory leaks

**Claude Code Feasibility:** ✅ **HIGH**

---

### Task 4.3: Documentation Updates

**Actions:**
- Update README with new platforms
- Update installation instructions
- Document build requirements
- Create migration guide

**Claude Code Feasibility:** ✅ **HIGH** - documentation is my strength

---

### Phase 4 Exit Criteria

✅ Modern toolchains supported (gcc 11+, clang 14+)
✅ Full test suite passes
✅ Documentation complete and accurate
✅ Installation tested on clean systems
✅ **PORT COMPLETE**

---

## Verification Strategy

### Continuous Verification

At every checkpoint:

```bash
# 1. Code compiles
make clean && make all
if [ $? -ne 0 ]; then
  echo "BUILD FAILED"
  exit 1
fi

# 2. Tests pass
cd tests && runtest --tool agents AGENTS=../agents
grep "# of expected passes" agents.sum

# 3. Smoke test
echo "write('Hello from AGENTS'), nl." | ../agents -b ../environment/boot.pam
```

### Automated Verification Script

```bash
#!/bin/bash
# verify-checkpoint.sh

set -e

echo "=== Building ==="
make clean && make all

echo "=== Running Tests ==="
cd tests
runtest --tool agents AGENTS=../agents
cd ..

echo "=== Smoke Test ==="
echo "write('Port verification OK'), nl." | ./agents -b environment/boot.pam

echo "=== SUCCESS ==="
exit 0
```

**Claude Code Usage:**
- Run this script after every change
- Parse output for success/failure
- Commit only if verification passes

---

## Session Management

### State Persistence

**Between sessions, Claude Code must:**
1. **Commit all changes to git**
2. **Write session log** (`session-NNNN.md`)
3. **Update progress tracker** (`PORTING-STATUS.md`)
4. **Save verification results** (`verification-YYYY-MM-DD.log`)

**On session start:**
1. **Read PORTING-STATUS.md** to understand current state
2. **Review recent session logs**
3. **Verify environment is clean**
4. **Resume from last checkpoint**

### Session Structure

**Typical session:**
```
1. Load context (read status files)
2. Execute 1-3 tasks (small, verifiable)
3. Run verification script
4. Commit changes
5. Update status
6. Document decisions
```

### Checkpoint Commits

Format:
```
Port checkpoint: <phase>.<task> - <description>

<what changed>
<verification results>
<next steps>
```

Example:
```
Port checkpoint: 2a.3 - Replace deprecated getwd() with getcwd()

Modified files:
- emulator/inout.c: Replace getwd() calls

Verification:
- Build: SUCCESS
- Tests: 37/37 PASS (no regression)

Next: Continue Stage 2a modernization
```

---

## Rollback and Recovery

### If Verification Fails

```bash
# Immediately rollback
git reset --hard HEAD~1

# Analyze failure
cat build.log
grep FAIL tests/agents.sum

# Document failure
echo "Approach X failed because Y" >> FAILURES.md

# Try alternative approach
```

### Saved States

At each phase boundary:
```bash
git tag phase-1-complete
git push --tags

# Also save external artifacts
tar czf phase-1-artifacts.tar.gz tests/*.sum tests/*.log build.log
```

---

## Critical Feasibility Analysis

### What is FEASIBLE for Claude Code ✅

**Phases that Claude Code can execute autonomously:**

1. **Code analysis and modification** (Phases 2-4)
   - ✅ Update platform detection code
   - ✅ Replace deprecated functions
   - ✅ Add HARDREGS definitions
   - ✅ Fix compiler warnings
   - ✅ Update documentation

2. **Native x86-64 building and testing** (Phase 3.3, Phase 4)
   - ✅ Run configure and make on native system
   - ✅ Execute test suite
   - ✅ Verify results
   - ✅ Iterate on failures

3. **Static analysis and planning** (All phases)
   - ✅ Understand codebase structure
   - ✅ Identify dependencies
   - ✅ Plan modifications
   - ✅ Document extensively

### What is CHALLENGING for Claude Code ⚠️

**Phases that require careful automation:**

1. **QEMU emulator setup** (Phase 0)
   - ⚠️ Downloading OS images (may need human to provide)
   - ⚠️ Interactive OS installation (needs expect scripts)
   - ⚠️ Network configuration (finicky)
   - **SOLUTION:** Use pre-built images or have human do initial setup

2. **Building inside QEMU** (Phase 1)
   - ⚠️ Long build times (10-30 minutes)
   - ⚠️ Timeout management
   - **SOLUTION:** Background builds with polling

3. **ARM64 native testing** (Phase 3.4)
   - ⚠️ May not have ARM64 environment available
   - **SOLUTION:** Cross-compile only, defer native testing or use QEMU

### What is NOT FEASIBLE for Claude Code ❌

**Operations that require human intervention:**

1. **Interactive debugging**
   - ❌ Cannot step through gdb interactively
   - ❌ Cannot investigate crashes in real-time
   - **SOLUTION:** Use verbose logging, core dumps, automated analysis

2. **Multi-hour operations**
   - ❌ Cannot wait for 2-hour compilation
   - ❌ Cannot babysit long-running tests
   - **SOLUTION:** Break into chunks, use background jobs

3. **Handling unexpected failures**
   - ❌ May encounter unknown build errors
   - ❌ May hit environment-specific issues
   - **SOLUTION:** Extensive documentation review, systematic debugging, human escalation if stuck

### Recommended Hybrid Approach

**Human does:**
- Phase 0: Set up QEMU environment with SSH access
- Provide any hard-to-obtain resources (OS images, etc.)
- Handle truly interactive debugging if Claude Code gets stuck

**Claude Code does:**
- Everything else: code analysis, modification, building, testing
- Systematic verification at each step
- Comprehensive documentation
- 90%+ of the actual porting work

**Result:** Efficient collaboration where each party does what they do best

---

## Alternative: Skip Vintage Emulation

### Direct Modernization Path

If QEMU setup proves too complex:

**Phase 1: Static Analysis Port**
1. Analyze Alpha 64-bit port (known good)
2. Apply same patterns to x86-64
3. Cross-reference with SPARC for any discrepancies
4. Make all code changes based on static analysis

**Phase 2: Native Build and Fix**
1. Attempt build on native x86-64
2. Fix errors one by one
3. Refer to documentation when stuck
4. Iterate until builds succeed

**Phase 3: Testing and Validation**
1. Run test suite
2. Fix failures
3. Verify against expected behavior

**Tradeoff:**
- ✅ **Faster** - no emulator setup
- ✅ **Simpler** - fewer moving parts
- ❌ **Higher risk** - no baseline
- ❌ **Harder debugging** - no comparison point

**When to use:** If vintage environment proves impractical

---

## References

### Source Documents

**Strategy Foundation:**
- `06-BUILD-SYSTEM.md` - Vintage Emulation Strategy (Section 9.3)
- `03-PLATFORM-DEPENDENCIES.md` - Alpha port as reference
- `08-RISK-ASSESSMENT.md` - Risk mitigation strategies

**Technical Details:**
- `01-ARCHITECTURE-OVERVIEW.md` - System architecture
- `02-EMULATOR-ANALYSIS.md` - Emulator internals
- `04-MEMORY-MANAGEMENT.md` - Memory model
- `05-BYTECODE-DISPATCH.md` - Instruction set
- `07-TESTING-STRATEGY.md` - Test methodology
- `10-COMPATIBILITY-MATRIX.md` - Platform comparison

**Overall Plan:**
- `STUDY-PLAN.md` - Porting study overview

---

## Revision History

| Date | Version | Changes |
|------|---------|---------|
| 2025-11-05 | 1.0 | Initial human-centric roadmap |
| 2025-11-05 | 2.0 | Complete rewrite for Claude Code execution with Vintage Emulation Strategy |

---

**End of Document**
