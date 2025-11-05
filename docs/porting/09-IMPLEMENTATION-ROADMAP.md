# AGENTS v0.9 - Implementation Roadmap

**Document:** Phase 5, Document 9 of 10
**Status:** Complete
**Date:** 2025-11-05
**Purpose:** Detailed phased implementation plan with milestones and effort estimates

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Implementation Philosophy](#implementation-philosophy)
3. [Phase 0: Preparation](#phase-0-preparation)
4. [Phase 1: Bootstrap Port](#phase-1-bootstrap-port)
5. [Phase 2: Optimization](#phase-2-optimization)
6. [Phase 3: Validation](#phase-3-validation)
7. [Phase 4: Production Ready](#phase-4-production-ready)
8. [Effort Estimates](#effort-estimates)
9. [Resource Requirements](#resource-requirements)
10. [Dependencies and Prerequisites](#dependencies-and-prerequisites)
11. [Success Metrics](#success-metrics)
12. [Risk Mitigation](#risk-mitigation)
13. [References](#references)

---

## Executive Summary

Based on the comprehensive analysis in documents 01-08, this roadmap outlines a **4-phase implementation strategy** for porting AGENTS v0.9 to modern x86-64 and ARM64 architectures.

### Key Finding: Low-Risk Port ✅

The existence of the **Alpha 64-bit port** significantly reduces porting risk. We can follow a proven path rather than blazing a new trail.

### Timeline Summary

| Phase | Duration | Effort | Status |
|-------|----------|--------|--------|
| Phase 0: Preparation | 1-2 weeks | 40-80 hours | 🔴 Not Started |
| Phase 1: Bootstrap | 2-4 weeks | 80-160 hours | 🔴 Not Started |
| Phase 2: Optimization | 2-3 weeks | 60-120 hours | 🔴 Not Started |
| Phase 3: Validation | 2-3 weeks | 60-120 hours | 🔴 Not Started |
| Phase 4: Production | 1-2 weeks | 40-80 hours | 🔴 Not Started |
| **TOTAL** | **8-14 weeks** | **280-560 hours** | **🔴** |

### Risk Level: **LOW-MEDIUM** 🟢

- Alpha port provides proven 64-bit foundation
- No fundamental architecture redesign needed
- Clear platform dependency boundaries
- Comprehensive existing test suite

---

## Implementation Philosophy

### Guiding Principles

1. **Minimal Disruption:** Change as little as possible
2. **Follow Alpha:** Use Alpha port as blueprint
3. **Test Early, Test Often:** Continuous validation
4. **Platform Parity:** x86-64 and ARM64 treated equally
5. **Incremental Progress:** Working system at each phase
6. **Preserve Backward Compatibility:** Keep existing ports working

### Strategy

**Bootstrap First, Optimize Later:**
- Phase 1: Get it working with conservative choices
- Phase 2: Add optimizations (HARDREGS, etc.)
- Phase 3: Validate correctness thoroughly
- Phase 4: Polish for production use

**Test-Driven Porting:**
- Port build system → Run tests → Fix failures → Repeat
- Use existing test suite as validation oracle
- Add architecture-specific tests as needed

---

## Phase 0: Preparation

**Duration:** 1-2 weeks
**Effort:** 40-80 hours
**Goal:** Set up development environment and infrastructure

### Tasks

#### 0.1 Development Environment Setup
**Effort:** 8-16 hours

**x86-64 Environment:**
- [ ] Provision x86-64 Linux development machine (Ubuntu 22.04+ or similar)
- [ ] Install gcc 11+ and clang 14+
- [ ] Install build dependencies: autoconf 2.69+, make, gmp-dev
- [ ] Install debugging tools: gdb, valgrind
- [ ] Install testing tools: DejaGnu 1.6+

**ARM64 Environment:**
- [ ] Provision ARM64 Linux machine (or QEMU emulation)
- [ ] Install same toolchain and dependencies as x86-64
- [ ] Configure cross-compilation if using QEMU
- [ ] Test basic compilation works

**Deliverables:**
- Working build environments for both platforms
- Documented setup procedures

---

#### 0.2 Baseline Build
**Effort:** 8-16 hours

**Goal:** Get existing codebase building on development machines (even if non-functional)

- [ ] Clone repository to both platforms
- [ ] Run `./configure` and document failures
- [ ] Update config.guess/config.sub to recognize modern platforms
- [ ] Fix immediate build blockers (missing headers, etc.)
- [ ] Achieve successful compilation (even with warnings/errors)

**Deliverables:**
- Build logs from both platforms
- List of compatibility issues encountered
- Initial patches for config.guess/config.sub

---

#### 0.3 Test Suite Familiarization
**Effort:** 16-24 hours

**Goal:** Understand existing test coverage and identify gaps

- [ ] Inventory all tests in `tests/` directory
- [ ] Run tests on a working platform (if available)
- [ ] Document test categories and coverage
- [ ] Identify architecture-specific tests
- [ ] Document test execution procedure

**From Document 07 (Testing Strategy):**
- ~60 test files across 9 categories
- DejaGnu framework (expect-based)
- Tests cover: syntax, stdlib, constraints, concurrency, I/O

**Deliverables:**
- Test suite inventory and categorization
- Test execution guide
- Baseline test results (if available from working platform)

---

#### 0.4 Source Code Repository Setup
**Effort:** 8-16 hours

**Goal:** Establish version control and branching strategy

- [ ] Create feature branches: `port/x86-64`, `port/arm64`
- [ ] Set up CI/CD pipeline (GitHub Actions, GitLab CI, etc.)
- [ ] Configure automated builds on both platforms
- [ ] Set up automated test runs
- [ ] Document branching and merging strategy

**Deliverables:**
- Configured CI/CD pipelines
- Branching strategy document
- Automated build/test infrastructure

---

#### 0.5 Reference Documentation
**Effort:** 8-16 hours

**Goal:** Consolidate and organize all analysis documents

- [ ] Review all porting study documents (01-10)
- [ ] Create quick reference guide for common tasks
- [ ] Document known issues and workarounds
- [ ] Prepare troubleshooting guide
- [ ] Set up documentation website (optional)

**Deliverables:**
- Consolidated reference documentation
- Quick start guide for porters
- Troubleshooting FAQ

---

### Phase 0 Exit Criteria

✅ Both platforms have working build environments
✅ Source code compiles (even if non-functional)
✅ Test suite is understood and executable
✅ CI/CD infrastructure is operational
✅ Team is familiar with codebase architecture

---

## Phase 1: Bootstrap Port

**Duration:** 2-4 weeks
**Effort:** 80-160 hours
**Goal:** Functional AGENTS system on x86-64 and ARM64

### Strategy

**Follow the Alpha Blueprint:**
1. Copy Alpha configuration patterns
2. Use conservative settings (PTR_ORG=0, no HARDREGS initially)
3. Focus on functional correctness, not performance
4. Validate with minimal test suite

---

### Tasks

#### 1.1 Platform Detection (Configure)
**Effort:** 16-24 hours

**Goal:** Teach configure script to recognize x86-64 and ARM64

**File:** `configure.in`

**Tasks:**
- [ ] Add x86-64 platform detection
- [ ] Add ARM64 platform detection
- [ ] Set platform variables (PLATFORM, TADBITS, etc.)
- [ ] Update config.guess and config.sub if needed
- [ ] Test configure on both platforms

**From Document 06 (Build System):**
```sh
# Add to configure.in
case "$host" in
  x86_64-*-linux*)
    PLATFORM=x86_64
    TADBITS=64
    ;;
  aarch64-*-linux*)
    PLATFORM=arm64
    TADBITS=64
    ;;
esac
```

**Deliverables:**
- Updated configure.in
- Successful configure runs on both platforms

---

#### 1.2 Platform Dependencies (sysdeps.h)
**Effort:** 24-40 hours

**Goal:** Define architecture parameters for x86-64 and ARM64

**File:** `emulator/sysdeps.h`

**Tasks:**
- [ ] Add x86-64 platform section (copy from Alpha)
- [ ] Add ARM64 platform section (copy from Alpha with adjustments)
- [ ] Set TADBITS=64 for both
- [ ] Set PTR_ORG=0 (like Alpha)
- [ ] Set WORDALIGNMENT=8
- [ ] Define feature flags (THREADED_CODE, etc.)
- [ ] Test compilation

**From Document 03 (Platform Dependencies):**
```c
#if defined(__x86_64__) || defined(__amd64__)
#define x86_64
#define TADBITS 64
#define PTR_ORG 0
#define WORDALIGNMENT 8
#define THREADED_CODE
// No HARDREGS initially
#endif

#if defined(__aarch64__) || defined(__arm64__)
#define arm64
#define TADBITS 64
#define PTR_ORG 0
#define WORDALIGNMENT 8
#define THREADED_CODE
#define OptionalWordAlign  // May need alignment helpers
#endif
```

**Deliverables:**
- Updated sysdeps.h with x86-64 and ARM64 sections
- Clean compilation on both platforms

---

#### 1.3 Register Allocation (regdefs.h) - OPTIONAL
**Effort:** 8-16 hours (if deferred to Phase 2)

**Goal:** Define soft register allocation (defer HARDREGS to Phase 2)

**File:** `emulator/regdefs.h`

**Tasks:**
- [ ] Add x86-64 section without HARDREGS
- [ ] Add ARM64 section without HARDREGS
- [ ] Use default soft register definitions
- [ ] Verify compilation

**Note:** HARDREGS can be added in Phase 2 for ~10-20% performance gain

**Deliverables:**
- Updated regdefs.h (minimal changes)
- Working soft register allocation

---

#### 1.4 Build System Integration
**Effort:** 16-24 hours

**Goal:** Complete build system support for new platforms

**Tasks:**
- [ ] Update all Makefile.in files with platform-specific rules
- [ ] Add platform-specific CFLAGS
- [ ] Add platform-specific LDFLAGS
- [ ] Test full build process (make clean && make)
- [ ] Verify all components compile: emulator, compiler, runtime

**From Document 06 (Build System):**
```makefile
# Makefile.in additions
ifeq ($(PLATFORM),x86_64)
  CFLAGS += -march=x86-64 -mtune=generic
endif

ifeq ($(PLATFORM),arm64)
  CFLAGS += -march=armv8-a
endif
```

**Deliverables:**
- Complete build succeeds on both platforms
- All binaries generated correctly

---

#### 1.5 Runtime System
**Effort:** 16-32 hours

**Goal:** Get compiler and core runtime working

**Tasks:**
- [ ] Build aklcomp (AKL compiler)
- [ ] Test compiling simple .akl programs
- [ ] Verify bytecode generation
- [ ] Check predicate tables
- [ ] Test basic agent loading

**Test Programs:**
```akl
% hello.akl
main :- write('Hello from AGENTS'), nl.
```

**Deliverables:**
- Working aklcomp on both platforms
- Successful compilation of test programs

---

#### 1.6 Basic Execution
**Effort:** 24-48 hours

**Goal:** Execute simple programs successfully

**Tasks:**
- [ ] Run emulator with minimal program
- [ ] Debug any startup failures
- [ ] Test basic instruction dispatch
- [ ] Verify threaded code works
- [ ] Test fallback to switch-based dispatch

**Test Sequence:**
1. **Minimal:** Program that just exits
2. **Simple I/O:** Print "hello world"
3. **Arithmetic:** Basic integer operations
4. **Unification:** Simple pattern matching
5. **Recursion:** Factorial or similar

**From Document 05 (Bytecode Dispatch):**
- Threaded code should work on both gcc and clang
- Switch-based dispatch as fallback
- Test both modes for correctness

**Deliverables:**
- Successful execution of minimal programs
- Debug logs showing correct instruction dispatch

---

#### 1.7 Memory Management Validation
**Effort:** 16-24 hours

**Goal:** Verify memory allocator and GC work correctly

**Tasks:**
- [ ] Test heap allocation
- [ ] Test stack allocation
- [ ] Force garbage collection
- [ ] Verify mark-sweep works
- [ ] Test for memory leaks (valgrind)

**From Document 04 (Memory Management):**
- Alpha port proves 64-bit GC works
- Mark bit in high bit (bit 63)
- WORDALIGNMENT=8 should prevent alignment faults

**Tests:**
- Large heap allocation
- Many small allocations (fragmentation test)
- GC stress test (allocate until GC triggers)

**Deliverables:**
- GC functioning correctly on both platforms
- No memory leaks detected
- Alignment issues resolved (ARM64)

---

### Phase 1 Exit Criteria

✅ Complete build system for x86-64 and ARM64
✅ Emulator executes simple programs correctly
✅ Basic test suite passes (20+ tests)
✅ Memory management validated
✅ No crashes or segfaults on core functionality
✅ Both platforms show parity in functionality

**Milestone:** First working AGENTS system on modern hardware! 🎉

---

## Phase 2: Optimization

**Duration:** 2-3 weeks
**Effort:** 60-120 hours
**Goal:** Optimize performance to match or exceed original

### Focus Areas

1. Hard register allocation (HARDREGS)
2. Compiler optimizations
3. Platform-specific tuning
4. Performance benchmarking

---

### Tasks

#### 2.1 Hard Register Allocation
**Effort:** 24-40 hours

**Goal:** Allocate frequently-used VM registers to physical registers

**File:** `emulator/regdefs.h`

**From Document 03 (Platform Dependencies):**
- Alpha uses 10 hard registers
- x86-64 has 16 GPRs → can use 10-12
- ARM64 has 31 GPRs → can use 10-15

**x86-64 Allocation (example):**
```c
#ifdef HARDREGS
#define Register register
register exstate *exs asm("%r12");
register code *pc asm("%r13");
register opcode op asm("%r14");
register Term *areg asm("%r15");
register Term *yreg asm("%rbx");
register andbox *andb asm("%rbp");
register choicebox *chb asm("%rdi");
register Reference *str asm("%rsi");
register int arity asm("%r8d");
register Term *heaplimit asm("%r9");
#endif
```

**ARM64 Allocation (example):**
```c
#ifdef HARDREGS
#define Register register
register exstate *exs asm("x19");
register code *pc asm("x20");
register opcode op asm("x21");
register Term *areg asm("x22");
register Term *yreg asm("x23");
register andbox *andb asm("x24");
register choicebox *chb asm("x25");
register Reference *str asm("x26");
register int arity asm("w27");
register Term *heaplimit asm("x28");
#endif
```

**Tasks:**
- [ ] Define HARDREGS for x86-64
- [ ] Define HARDREGS for ARM64
- [ ] Test compilation
- [ ] Run benchmarks
- [ ] Measure performance improvement

**Expected Gain:** 10-20% speedup

**Deliverables:**
- Hard register definitions for both platforms
- Performance measurements before/after

---

#### 2.2 Compiler Optimization Flags
**Effort:** 8-16 hours

**Goal:** Find optimal compiler flags for each platform

**From Document 06 (Build System):**

**x86-64 Flags:**
```makefile
CFLAGS += -O3 -march=x86-64-v2 -mtune=generic
CFLAGS += -fomit-frame-pointer
CFLAGS += -funroll-loops
```

**ARM64 Flags:**
```makefile
CFLAGS += -O3 -march=armv8-a
CFLAGS += -fomit-frame-pointer
CFLAGS += -funroll-loops
```

**Tasks:**
- [ ] Test various optimization levels (-O2 vs -O3)
- [ ] Test micro-architecture targeting
- [ ] Profile and identify hot paths
- [ ] Tune based on profiling results

**Deliverables:**
- Optimal CFLAGS for each platform
- Performance comparison matrix

---

#### 2.3 Platform-Specific Optimizations
**Effort:** 16-24 hours

**Goal:** Leverage platform-specific features

**x86-64 Opportunities:**
- Fast unaligned access (less critical)
- Large register file
- Strong memory ordering (fewer barriers)

**ARM64 Opportunities:**
- Large register file (31 regs)
- May need alignment helpers (OptionalWordAlign)
- May need memory barriers for concurrent code

**Tasks:**
- [ ] Review hot paths in profiler
- [ ] Add platform-specific optimizations where beneficial
- [ ] Test OptionalWordAlign on ARM64
- [ ] Add memory barriers if needed for concurrency

**Deliverables:**
- Platform-specific optimization patches
- Performance measurements

---

#### 2.4 Benchmarking Suite
**Effort:** 16-24 hours

**Goal:** Comprehensive performance benchmarking

**From Document 07 (Testing Strategy):**
- Use existing benchmarks in `benchmarks/` directory
- Add new benchmarks for architecture-sensitive operations

**Benchmark Categories:**
- Instruction dispatch overhead
- Memory allocation/GC speed
- Unification performance
- Concurrent execution (if applicable)
- I/O throughput

**Tasks:**
- [ ] Run all existing benchmarks
- [ ] Create new micro-benchmarks for critical paths
- [ ] Compare x86-64 vs ARM64 performance
- [ ] Compare against Alpha port (if available)
- [ ] Identify performance regressions

**Deliverables:**
- Comprehensive benchmark results
- Performance comparison report

---

### Phase 2 Exit Criteria

✅ HARDREGS implemented and validated
✅ Optimal compiler flags determined
✅ Platform-specific optimizations applied
✅ Performance matches or exceeds original targets
✅ No performance regressions vs Phase 1
✅ Benchmark suite established

**Milestone:** Production-grade performance achieved! 🚀

---

## Phase 3: Validation

**Duration:** 2-3 weeks
**Effort:** 60-120 hours
**Goal:** Comprehensive testing and validation

### Focus Areas

1. Full test suite execution
2. Stress testing
3. Concurrency validation
4. Edge case testing
5. Cross-platform consistency

---

### Tasks

#### 3.1 Full Test Suite Execution
**Effort:** 16-24 hours

**Goal:** Run all existing tests on both platforms

**From Document 07 (Testing Strategy):**
- ~60 test files in DejaGnu framework
- Categories: syntax, stdlib, constraints, concurrent, I/O

**Tasks:**
- [ ] Run complete test suite on x86-64
- [ ] Run complete test suite on ARM64
- [ ] Document all failures
- [ ] Fix failures (or document as known issues)
- [ ] Achieve >95% pass rate

**Expected Results:**
- Most tests should pass immediately (Alpha port similarity)
- Some tests may be platform-specific (document/skip)
- Some tests may reveal edge cases (fix)

**Deliverables:**
- Test results report for both platforms
- Failure analysis and fixes
- Updated test suite (if needed)

---

#### 3.2 Stress Testing
**Effort:** 16-24 hours

**Goal:** Test system under extreme conditions

**Test Categories:**
- **Memory stress:** Large heaps, many allocations, GC pressure
- **Deep recursion:** Stack depth limits
- **Long-running:** Hours/days of continuous execution
- **Concurrent load:** Many simultaneous agents (if applicable)

**Tasks:**
- [ ] Create stress test suite
- [ ] Run stress tests on both platforms
- [ ] Monitor for memory leaks (valgrind)
- [ ] Monitor for crashes
- [ ] Test recovery from resource exhaustion

**Deliverables:**
- Stress test suite
- Stability report

---

#### 3.3 Concurrency Validation
**Effort:** 16-24 hours

**Goal:** Validate concurrent constraint execution

**From Documents 01-02:**
- AGENTS uses or-parallelism (choice points)
- Guards and committed choice
- May use platform threading (unclear from study)

**Tasks:**
- [ ] Test concurrent agent execution
- [ ] Test guard commitment
- [ ] Test choice point handling
- [ ] Verify thread safety (if applicable)
- [ ] Test on multi-core systems

**Deliverables:**
- Concurrency test results
- Thread safety validation

---

#### 3.4 Edge Case Testing
**Effort:** 16-24 hours

**Goal:** Test boundary conditions and edge cases

**Test Cases:**
- **Large integers:** GMP integration
- **Deep structures:** Nested terms
- **Empty collections:** [], nil
- **Maximum sizes:** String limits, heap limits
- **Error conditions:** Out of memory, stack overflow
- **Platform limits:** Pointer sizes, alignment

**Tasks:**
- [ ] Design edge case test suite
- [ ] Run on both platforms
- [ ] Compare results for consistency
- [ ] Document any platform differences

**Deliverables:**
- Edge case test suite
- Platform comparison report

---

#### 3.5 Cross-Platform Consistency
**Effort:** 8-16 hours

**Goal:** Ensure x86-64 and ARM64 produce identical results

**Tasks:**
- [ ] Run same programs on both platforms
- [ ] Compare outputs (should be identical)
- [ ] Compare performance (within reasonable variance)
- [ ] Document any differences
- [ ] Investigate and fix inconsistencies

**Deliverables:**
- Cross-platform consistency report
- List of known differences (if any)

---

### Phase 3 Exit Criteria

✅ Full test suite passes on both platforms (>95%)
✅ Stress tests show stability
✅ Concurrency validated
✅ Edge cases handled correctly
✅ x86-64 and ARM64 show behavioral parity
✅ No critical bugs or crashes

**Milestone:** Production-ready quality achieved! ✅

---

## Phase 4: Production Ready

**Duration:** 1-2 weeks
**Effort:** 40-80 hours
**Goal:** Polish for production deployment

### Focus Areas

1. Documentation
2. Installation and packaging
3. User guides
4. Compatibility guarantees
5. Release preparation

---

### Tasks

#### 4.1 Documentation Updates
**Effort:** 16-24 hours

**Goal:** Update all documentation for new platforms

**Tasks:**
- [ ] Update README with x86-64/ARM64 support
- [ ] Update installation guide
- [ ] Update build instructions
- [ ] Document platform-specific notes
- [ ] Update man pages (if applicable)
- [ ] Create migration guide from SPARC

**Files:**
- README
- INSTALL
- doc/installation.md (if exists)
- doc/manual/ (user manual)

**Deliverables:**
- Updated documentation
- Platform support matrix
- Migration guide

---

#### 4.2 Installation and Packaging
**Effort:** 16-24 hours

**Goal:** Easy installation on target platforms

**Tasks:**
- [ ] Test `make install` on both platforms
- [ ] Create distribution packages (.tar.gz)
- [ ] Create platform packages (deb, rpm, homebrew)
- [ ] Test installation on clean systems
- [ ] Document installation process

**Platforms:**
- Ubuntu/Debian (apt packages)
- RHEL/CentOS (rpm packages)
- Arch Linux (AUR package)
- macOS (homebrew formula, if supporting macOS ARM64)

**Deliverables:**
- Working installation process
- Distribution packages
- Package repository setup (optional)

---

#### 4.3 Backwards Compatibility
**Effort:** 8-16 hours

**Goal:** Ensure existing platforms still work

**Tasks:**
- [ ] Test that SPARC build still works (if possible)
- [ ] Test that other existing platforms compile
- [ ] Verify no regressions introduced
- [ ] Document compatibility guarantees

**Deliverables:**
- Backwards compatibility report
- No regressions in existing platforms

---

#### 4.4 Release Preparation
**Effort:** 8-16 hours

**Goal:** Prepare for public release

**Tasks:**
- [ ] Create release notes
- [ ] Update version numbers
- [ ] Tag release in git
- [ ] Create release branch
- [ ] Prepare announcement

**Release Notes Should Include:**
- New platform support (x86-64, ARM64)
- Performance improvements
- Bug fixes
- Known issues
- Migration guide
- Credits and acknowledgments

**Deliverables:**
- Release notes
- Tagged release
- Release announcement

---

### Phase 4 Exit Criteria

✅ Documentation complete and accurate
✅ Installation process tested and documented
✅ Packages available for major distributions
✅ Backwards compatibility verified
✅ Release prepared and tagged

**Milestone:** Ready for public release! 🎉

---

## Effort Estimates

### Summary Table

| Phase | Min (hours) | Max (hours) | Min (weeks) | Max (weeks) |
|-------|-------------|-------------|-------------|-------------|
| Phase 0: Preparation | 40 | 80 | 1 | 2 |
| Phase 1: Bootstrap | 80 | 160 | 2 | 4 |
| Phase 2: Optimization | 60 | 120 | 1.5 | 3 |
| Phase 3: Validation | 60 | 120 | 1.5 | 3 |
| Phase 4: Production | 40 | 80 | 1 | 2 |
| **TOTAL** | **280** | **560** | **7** | **14** |

**Assumptions:**
- 1 full-time developer
- 40 hours/week
- 8 hours/day

**Confidence Level:**
- **Minimum (280 hours):** Optimistic but achievable with experienced team
- **Maximum (560 hours):** Conservative, accounts for unknowns and setbacks
- **Expected (420 hours):** Realistic middle ground ≈ 10-11 weeks

---

### Effort by Task Type

| Task Type | Hours | % of Total |
|-----------|-------|------------|
| Build System | 60-120 | 21-21% |
| Platform Configuration | 50-100 | 18-18% |
| Testing & Validation | 80-160 | 29-29% |
| Optimization | 40-80 | 14-14% |
| Documentation | 30-60 | 11-11% |
| Infrastructure | 20-40 | 7-7% |
| **TOTAL** | **280-560** | **100%** |

---

## Resource Requirements

### Personnel

**Minimum Team:**
- 1 experienced C programmer with VM/compiler knowledge
- Part-time access to architecture expert (ARM64 specifics)
- Part-time tester

**Ideal Team:**
- 1 lead developer (VM internals)
- 1 platform engineer (build systems, architecture)
- 1 test engineer
- 1 documentation writer (part-time)

**Skills Required:**
- Strong C programming
- Understanding of VM/interpreter design
- Experience with x86-64 and/or ARM64
- Build system expertise (autoconf, make)
- Debugging skills (gdb, valgrind)
- Git version control

---

### Hardware

**Required:**
- x86-64 Linux development machine (8+ GB RAM, 50+ GB disk)
- ARM64 Linux development machine or QEMU emulator
  - Raspberry Pi 4 (8GB) acceptable
  - AWS Graviton instance
  - Apple Silicon Mac (if macOS support desired)

**Nice to Have:**
- Multiple x86-64 machines (different vendors/models)
- Multiple ARM64 devices (different vendors)
- Older hardware for compatibility testing

---

### Software

**Required:**
- gcc 11+ or clang 14+
- autoconf 2.69+, automake, libtool
- GNU make
- GMP library (libgmp-dev)
- Git
- gdb
- DejaGnu 1.6+

**Nice to Have:**
- valgrind
- perf (performance profiling)
- Multiple compiler versions (compatibility testing)
- Docker (for reproducible builds)
- CI/CD platform (GitHub Actions, GitLab CI)

---

## Dependencies and Prerequisites

### Critical Path Dependencies

```
Phase 0: Preparation
    ↓
Phase 1: Bootstrap ← Must complete before optimization
    ↓
Phase 2: Optimization ← Can overlap with Phase 3
    ↓
Phase 3: Validation ← Must complete before release
    ↓
Phase 4: Production
```

### External Dependencies

**Low Risk:**
- GMP library availability (widely available)
- Compiler support for computed goto (gcc/clang standard)
- DejaGnu test framework (mature and stable)

**Medium Risk:**
- Access to ARM64 hardware (cloud instances available)
- Availability of original test suite (appears complete)
- Documentation of original design (study docs fill gaps)

**Blockers (if any):**
- None identified (all critical dependencies available)

---

## Success Metrics

### Functional Correctness
- ✅ Full test suite passes (>95% pass rate)
- ✅ No crashes or segfaults on valid programs
- ✅ Identical behavior across x86-64 and ARM64
- ✅ GC operates correctly (no leaks)
- ✅ Concurrent execution works (if applicable)

### Performance
- ✅ Threaded code dispatch works on both platforms
- ✅ Performance within 80-120% of Alpha port (if comparable)
- ✅ HARDREGS provides 10-20% speedup
- ✅ No major performance regressions

### Quality
- ✅ Clean compilation (no warnings with -Wall)
- ✅ Valgrind clean (no memory errors)
- ✅ Passes static analysis (if used)
- ✅ Code follows existing style

### Usability
- ✅ Simple build process (`./configure && make`)
- ✅ Clear documentation
- ✅ Easy installation
- ✅ Works on major Linux distributions

---

## Risk Mitigation

### Technical Risks

**Risk:** Alignment issues on ARM64
**Likelihood:** Medium
**Impact:** Medium
**Mitigation:**
- Use OptionalWordAlign from the start
- Test early on ARM64 hardware
- Have alignment helper functions ready

**Risk:** Performance regression with HARDREGS
**Likelihood:** Low
**Impact:** Medium
**Mitigation:**
- Benchmark before and after
- Have soft register fallback
- Profile to identify issues

**Risk:** Test suite failures
**Likelihood:** Medium
**Impact:** High
**Mitigation:**
- Start testing early in Phase 1
- Fix failures incrementally
- Maintain test results log

---

### Schedule Risks

**Risk:** Unforeseen architecture issues
**Likelihood:** Low
**Impact:** High
**Mitigation:**
- Alpha port proves 64-bit feasibility
- Buffer time in estimates (280-560 hour range)
- Incremental approach allows early detection

**Risk:** Resource unavailability (hardware, personnel)
**Likelihood:** Medium
**Impact:** Medium
**Mitigation:**
- Use cloud instances for ARM64 if needed
- Document all steps for handoffs
- Cross-train team members

---

### Quality Risks

**Risk:** Incomplete testing
**Likelihood:** Medium
**Impact:** High
**Mitigation:**
- Dedicated Phase 3 for validation
- Use existing comprehensive test suite
- Add architecture-specific tests

**Risk:** Documentation gaps
**Likelihood:** Low
**Impact:** Medium
**Mitigation:**
- Dedicated Phase 4 for documentation
- Porting study docs provide foundation
- Review by independent reader

---

## References

### Porting Study Documents

**Phase 1: Architecture**
- 01-ARCHITECTURE-OVERVIEW.md - System design
- 02-EMULATOR-ANALYSIS.md - VM internals
- 03-PLATFORM-DEPENDENCIES.md - Architecture inventory

**Phase 2: Technical**
- 04-MEMORY-MANAGEMENT.md - Memory model and GC
- 05-BYTECODE-DISPATCH.md - Instruction dispatch

**Phase 3: Build System**
- 06-BUILD-SYSTEM.md - Build system analysis

**Phase 4: Testing**
- 07-TESTING-STRATEGY.md - Test approach

**Phase 5: Planning**
- 08-RISK-ASSESSMENT.md - Risk analysis
- 10-COMPATIBILITY-MATRIX.md - Platform compatibility

---

### Key Source Files

**Configuration:**
- configure.in - Build configuration
- emulator/sysdeps.h - Platform parameters
- emulator/regdefs.h - Register allocation

**Core Emulator:**
- emulator/engine.c - Instruction interpreter
- emulator/instructions.h - Dispatch macros
- emulator/opcodes.h - Instruction set

**Build System:**
- Makefile.in - Top-level build
- emulator/Makefile.in - Emulator build

---

## Appendix: Alpha Port Reference

### Why Alpha is the Blueprint

The DEC Alpha port provides a **proven 64-bit implementation**:

**Alpha Characteristics:**
- 64-bit architecture (like x86-64, ARM64)
- Little-endian (like x86-64, ARM64)
- RISC design (like ARM64)
- Large register file (like ARM64)

**Alpha Configuration (from sysdeps.h):**
```c
#ifdef alpha
#define TADBITS 64
#define PTR_ORG 0
#define WORDALIGNMENT 8
#define THREADED_CODE
#ifdef HARDREGS
#define Register register
register exstate *exs asm("$9");
register code *pc asm("$10");
// ... 10 hard registers total
#endif
#endif
```

**Lessons from Alpha:**
1. PTR_ORG=0 works (no pointer adjustment)
2. WORDALIGNMENT=8 sufficient
3. HARDREGS improves performance
4. Threaded code works with RISC
5. GC mark bit in high bit (bit 63)

**Differences to Address:**
- x86-64 is CISC (vs RISC) - minimal impact
- ARM64 may need OptionalWordAlign
- Register names differ (use gcc syntax)

---

## Revision History

| Date | Version | Changes |
|------|---------|---------|
| 2025-11-05 | 1.0 | Initial implementation roadmap |

---

**End of Document**
