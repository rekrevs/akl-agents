# AGENTS v0.9 - Modern Architecture Porting Study Plan

**Document Version:** 3.0
**Last Updated:** 2025-11-05
**Branch:** `claude/port-attempt-01-011CUoHSMTFawkiKfVsuVuPP`
**Status:** ✅ **PLANNING COMPLETE** → 🟡 **IMPLEMENTATION PHASE**

---

## Executive Summary

This document outlines the comprehensive study plan for analyzing and documenting the porting requirements of the AGENTS v0.9 concurrent constraint programming system to modern x86-64 and ARM64 architectures. The study focuses on **documentation only** - no code will be modified during this phase.

### System Overview
- **Name:** AGENTS v0.9
- **Type:** Concurrent constraint programming language system
- **Origin:** Swedish Institute of Computer Science (SICS)
- **Current Primary Target:** SPARC (with GMP support for multiple architectures)
- **Target Architectures:** Modern x86-64 (Intel/AMD) and ARM64 (ARMv8+)

### Key Components
1. **Emulator** (3.3M) - Core execution engine in C
2. **GMP Library** (14M) - GNU Multiple Precision arithmetic (excluded from this study)
3. **Compiler** (614K) - AKL-to-bytecode compiler
4. **Runtime** (546K) - Built-in agents and environment
5. **Libraries** (3.7M) - User-level packages
6. **Documentation** (1.1M) - Manuals and reference

---

## Objectives

### Primary Goals
1. **Identify** all architecture-dependent code paths in the emulator
2. **Document** current design decisions and their architectural implications
3. **Analyze** compatibility with modern x86-64 and ARM64 platforms
4. **Plan** the porting strategy with minimal disruption to existing code
5. **Assess** risks, effort estimates, and testing requirements
6. **Create** comprehensive documentation for future implementation

### Exclusions
- **GMP Library:** Will be handled separately (already has modern x86/ARM support)
- **Code Implementation:** This study is documentation-only
- **Performance Tuning:** Focus is on functional correctness, not optimization

---

## Documentation Structure

All documentation will be created in the `docs/porting/` directory:

```
docs/porting/
├── 01-ARCHITECTURE-OVERVIEW.md      [✅ COMPLETE]
│   └── High-level system architecture and execution model
├── 02-EMULATOR-ANALYSIS.md          [✅ COMPLETE]
│   └── Detailed emulator code analysis
├── 03-PLATFORM-DEPENDENCIES.md      [✅ COMPLETE]
│   └── Inventory of platform-specific code
├── 04-MEMORY-MANAGEMENT.md          [✅ COMPLETE]
│   └── Memory model, alignment, and GC analysis
├── 05-BYTECODE-DISPATCH.md          [✅ COMPLETE]
│   └── Threaded code and instruction dispatch
├── 06-BUILD-SYSTEM.md               [✅ COMPLETE]
│   └── Autoconf/Make modernization requirements
├── 07-TESTING-STRATEGY.md           [✅ COMPLETE]
│   └── Test framework and validation approach
├── 08-RISK-ASSESSMENT.md            [✅ COMPLETE]
│   └── Risks, challenges, and mitigation strategies
├── 09-IMPLEMENTATION-ROADMAP.md     [✅ COMPLETE]
│   └── Claude Code implementation roadmap with Vintage Emulation Strategy
├── 10-COMPATIBILITY-MATRIX.md       [✅ COMPLETE]
│   └── Feature/platform compatibility tracking
├── 11-CLAUDE-CODE-REALITY-CHECK.md  [✅ COMPLETE]
│   └── Honest feasibility assessment for Claude Code execution
├── 12-OPTION-B-EXECUTION-PLAN.md    [✅ COMPLETE]
│   └── Detailed step-by-step execution plan for Direct Modern Port
└── 13-ITERATIVE-STRATEGY.md         [✅ COMPLETE]
    └── Framework for learning from failures and iterating

docs/attempts/
├── README.md                         [✅ COMPLETE]
│   └── Attempt tracking overview
├── LESSONS-LEARNED.md                [✅ COMPLETE]
│   └── Cumulative knowledge across all attempts
└── ATTEMPT-01/                       [🟡 READY]
    ├── ATTEMPT-01-PLAN.md            [✅ COMPLETE]
    ├── ATTEMPT-01-LOG.md             [📝 Template ready]
    ├── ATTEMPT-01-ANALYSIS.md        [📝 Template ready]
    └── ATTEMPT-01-RESULTS.md         [📝 Template ready]
```

---

## Study Phases

### Phase 1: System Architecture Analysis [✅ COMPLETE]
**Duration:** 3 documents
**Status:** 🟢 Complete

**Deliverables:**
- [x] `01-ARCHITECTURE-OVERVIEW.md` - System architecture deep-dive (✅ DONE)
- [x] `02-EMULATOR-ANALYSIS.md` - Emulator internals documentation (✅ DONE)
- [x] `03-PLATFORM-DEPENDENCIES.md` - Complete inventory of arch-specific code (✅ DONE)

**Key Questions to Answer:**
- What is the virtual machine execution model?
- How does bytecode interpretation work?
- What are the core data structures and their memory layouts?
- Where are the architecture decision points?
- What threading/concurrency model is used?

**Files to Analyze:**
- `emulator/sysdeps.h` - Platform detection and dependencies
- `emulator/regdefs.h` - Register allocation strategies
- `emulator/*.c` - Core emulator implementation (~50 files)
- `emulator/*.h` - Header files and interfaces (~63 files)
- `configure.in` - Build-time architecture detection

---

### Phase 2: Technical Deep Dives [✅ COMPLETE]
**Duration:** 2 documents
**Status:** 🟢 Complete

**Deliverables:**
- [x] `04-MEMORY-MANAGEMENT.md` - Memory model and alignment (✅ DONE)
- [x] `05-BYTECODE-DISPATCH.md` - Instruction dispatch mechanisms (✅ DONE)

**Key Questions to Answer:**
- How are memory addresses calculated and used?
- What alignment requirements exist?
- How does threaded code dispatch work (computed goto vs switch)?
- Are there any hand-coded assembly sections?
- What are the endianness assumptions?
- How does the garbage collector interact with architecture?

**Files to Analyze:**
- Memory allocator implementations
- Garbage collector code
- Bytecode interpreter main loop
- Signal handlers and system calls
- Any inline assembly or architecture intrinsics

---

### Phase 3: Build System and Toolchain [✅ COMPLETE]
**Duration:** 1 document
**Status:** 🟢 Complete

**Deliverables:**
- [x] `06-BUILD-SYSTEM.md` - Build system modernization plan (✅ DONE)

**Key Questions to Answer:**
- What compiler features are required (computed goto, extensions)?
- How does the build system detect architectures?
- What are the configure-time checks needed?
- Are there any obsolete build assumptions?
- What external dependencies exist?

**Files to Analyze:**
- `configure.in`, `configure`
- `Makefile.in` and subdirectory Makefiles
- `config.guess`, `config.sub`
- Compiler feature detection macros

---

### Phase 4: Testing and Validation Strategy [✅ COMPLETE]
**Duration:** 1 document
**Status:** 🟢 Complete

**Deliverables:**
- [x] `07-TESTING-STRATEGY.md` - Comprehensive testing plan (✅ DONE)

**Key Questions to Answer:**
- What test frameworks are available?
- What is the current test coverage?
- How can we validate correctness across architectures?
- What benchmarks should be used?
- Are there architecture-specific test requirements?

**Files to Analyze:**
- `tests/` directory - DejaGnu test suite
- `benchmarks/` directory
- `demos/` directory - Example programs as test cases

---

### Phase 5: Risk Assessment and Planning [✅ COMPLETE]
**Duration:** 3 documents
**Status:** 🟢 Complete

**Deliverables:**
- [x] `08-RISK-ASSESSMENT.md` - Risks and mitigation strategies (✅ DONE)
- [x] `09-IMPLEMENTATION-ROADMAP.md` - Detailed phased implementation plan (✅ DONE)
- [x] `10-COMPATIBILITY-MATRIX.md` - Feature/platform compatibility tracking (✅ DONE)

**Key Questions to Answer:**
- What are the highest-risk components?
- What is the effort estimate for porting?
- What is the testing burden?
- What are the failure modes?
- What is the rollback strategy?
- What skills/expertise are required?

---

## Analysis Methodology

### Code Analysis Approach
1. **Static Analysis:** Read source code to understand design
2. **Documentation Review:** Study existing docs and comments
3. **Pattern Recognition:** Identify architecture-specific patterns
4. **Dependency Mapping:** Track data and control flow dependencies
5. **Risk Flagging:** Mark high-risk/high-complexity areas

### Documentation Standards
- **Clarity:** Write for future implementers unfamiliar with the system
- **Completeness:** Document assumptions, rationale, and alternatives
- **Traceability:** Reference source files and line numbers
- **Actionability:** Provide concrete recommendations, not just observations
- **Maintenance:** Keep this STUDY-PLAN.md updated with current status

### File Reference Format
When referencing code, use: `filename:line_number` or `filename:function_name`

Example: `emulator/sysdeps.h:45` or `emulator/eval.c:execute_instruction()`

---

## Progress Tracking

### Overall Status: 🟢 ALL PLANNING COMPLETE (100% Complete) ✅ → 🟡 Implementation Starting

| Phase | Documents | Status | Completion |
|-------|-----------|--------|------------|
| Phase 1: Architecture | 3 docs | 🟢 Complete | 3/3 (100%) |
| Phase 2: Technical Deep Dives | 2 docs | 🟢 Complete | 2/2 (100%) |
| Phase 3: Build System | 1 doc | 🟢 Complete | 1/1 (100%) |
| Phase 4: Testing Strategy | 1 doc | 🟢 Complete | 1/1 (100%) |
| Phase 5: Risk & Planning | 3 docs | 🟢 Complete | 3/3 (100%) |
| **Phase 6: Execution Planning** | **2 docs** | **🟢 Complete** | **2/2 (100%)** |
| **Supplemental** | **1 doc** | **🟢 Complete** | **1/1 (100%)** |
| **Attempt Infrastructure** | **4 docs** | **🟢 Ready** | **4/4 (100%)** |
| **TOTAL PLANNING** | **13 docs + infra** | **🟢** | **13/13 (100%)** ✅ |

### Status Legend
- 🔴 Not Started
- 🟡 In Progress
- 🟢 Complete
- 🔵 Under Review
- ⚪ Blocked/Deferred

---

## Document Status Tracking

### Phase 1 Documents
- [x] **01-ARCHITECTURE-OVERVIEW.md** - 🟢 Complete
- [x] **02-EMULATOR-ANALYSIS.md** - 🟢 Complete
- [x] **03-PLATFORM-DEPENDENCIES.md** - 🟢 Complete

### Phase 2 Documents
- [x] **04-MEMORY-MANAGEMENT.md** - 🟢 Complete
- [x] **05-BYTECODE-DISPATCH.md** - 🟢 Complete

### Phase 3 Documents
- [x] **06-BUILD-SYSTEM.md** - 🟢 Complete

### Phase 4 Documents
- [x] **07-TESTING-STRATEGY.md** - 🟢 Complete

### Phase 5 Documents
- [x] **08-RISK-ASSESSMENT.md** - 🟢 Complete
- [x] **09-IMPLEMENTATION-ROADMAP.md** - 🟢 Complete (v2.0 - Claude Code focused)
- [x] **10-COMPATIBILITY-MATRIX.md** - 🟢 Complete

### Phase 6 Documents (Execution Planning)
- [x] **12-OPTION-B-EXECUTION-PLAN.md** - 🟢 Complete (67 detailed steps)
- [x] **13-ITERATIVE-STRATEGY.md** - 🟢 Complete (Learning framework)

### Supplemental Documents
- [x] **11-CLAUDE-CODE-REALITY-CHECK.md** - 🟢 Complete (Feasibility analysis)

### Attempt Infrastructure
- [x] **docs/attempts/README.md** - 🟢 Complete
- [x] **docs/attempts/LESSONS-LEARNED.md** - 🟢 Complete
- [x] **docs/attempts/ATTEMPT-01/ATTEMPT-01-PLAN.md** - 🟢 Complete
- [x] **docs/attempts/ATTEMPT-01/templates** - 🟢 Ready

---

## Key Areas of Investigation

### Critical Components Requiring Analysis

#### 1. Emulator Core
- **Priority:** 🔴 Critical
- **Files:** `emulator/*.c`, `emulator/*.h`
- **Focus Areas:**
  - Virtual machine execution model
  - Bytecode interpretation loop
  - Threaded code dispatch mechanism
  - Register allocation (soft vs hard registers)
  - Stack frame management
  - Choice point and continuation handling

#### 2. Platform Dependencies
- **Priority:** 🔴 Critical
- **Files:** `emulator/sysdeps.h`, `emulator/regdefs.h`
- **Focus Areas:**
  - Platform detection macros
  - Architecture-specific register definitions
  - Memory addressing assumptions
  - Alignment requirements
  - Endianness handling

#### 3. Memory Management
- **Priority:** 🔴 Critical
- **Files:** Memory allocator, GC, heap management
- **Focus Areas:**
  - Heap layout and addressing
  - Garbage collection algorithm
  - Memory alignment requirements
  - Pointer tagging schemes
  - Double-word operations

#### 4. Build System
- **Priority:** 🟡 Important
- **Files:** `configure.in`, `Makefile.in`, `config.*`
- **Focus Areas:**
  - Architecture detection
  - Compiler feature detection
  - Tool requirements
  - Installation paths

#### 5. Runtime System
- **Priority:** 🟡 Important
- **Files:** `compiler/`, `environment/`
- **Focus Areas:**
  - Compiler assumptions about target
  - Bytecode format dependencies
  - Built-in agents and primitives

#### 6. Testing Infrastructure
- **Priority:** 🟡 Important
- **Files:** `tests/`, `benchmarks/`
- **Focus Areas:**
  - Test coverage
  - Architecture-specific tests
  - Regression test suite
  - Performance benchmarks

---

## Target Platform Specifications

### Modern x86-64 (Intel/AMD)
- **Architecture:** x86-64 (AMD64/Intel 64)
- **Word Size:** 64-bit
- **Endianness:** Little-endian
- **Alignment:** Relaxed (performance penalty for unaligned access)
- **Key Features:**
  - Large register set (16 GPRs, 16 SSE/AVX registers)
  - Computed goto support in gcc/clang
  - Mature compiler support
  - Strong memory model

### Modern ARM64 (ARMv8+)
- **Architecture:** ARMv8-A or later
- **Word Size:** 64-bit
- **Endianness:** Little-endian (typically, bi-endian capable)
- **Alignment:** Strict (unaligned access may fault)
- **Key Features:**
  - 31 general-purpose registers
  - Load/store architecture
  - Computed goto support in gcc/clang
  - Weaker memory model (requires barriers)

### Commonalities
- Both are 64-bit architectures (vs. original 32-bit SPARC)
- Both support computed goto (required for threaded code)
- Both have little-endian as primary mode
- Both have mature gcc/clang support
- Both may have existing GMP optimizations

---

## Success Criteria

### Study Completion Criteria
This porting study will be considered complete when:

1. ✅ All 14 documentation files are completed
2. ✅ All critical architecture dependencies are identified and documented
3. ✅ A comprehensive risk assessment is available
4. ✅ A detailed implementation roadmap exists with effort estimates
5. ✅ Testing strategy is defined with validation criteria
6. ✅ Build system requirements are documented
7. ✅ All documentation is peer-reviewed for accuracy and completeness

### Documentation Quality Standards
Each document must:
- Be technically accurate with source code references
- Provide actionable recommendations
- Identify risks and mitigation strategies
- Include diagrams where helpful
- Be understandable to someone unfamiliar with the codebase
- Be maintained as findings evolve

---

## Timeline and Milestones

### Proposed Schedule
*(To be refined as work progresses)*

- **Week 1:** Phase 1 completion (Architecture analysis)
- **Week 2:** Phase 2 completion (Technical deep dives)
- **Week 3:** Phase 3-4 completion (Build system, testing)
- **Week 4:** Phase 5 completion (Risk assessment, roadmap)
- **Week 5:** Review, refinement, finalization

**Note:** This is a rough estimate. Actual timeline depends on:
- Complexity discovered during analysis
- Availability of domain expertise
- Clarifications needed from original developers
- Scope adjustments

---

## Resources and References

### Internal Documentation
- `README` - System overview and building instructions
- `doc/` - User and reference manuals
- `doc/implementation/` - Implementation notes (if exists)

### External References
- AGENTS papers and publications from SICS
- GMP documentation and architecture guides
- Warren's Abstract Machine (WAM) - similar VM design
- Threaded code interpretation techniques
- Modern compiler documentation (GCC, Clang)

### Tools Required
- Source code browsers (cscope, ctags, or IDE)
- Diagram tools (for architecture diagrams)
- Build tools (autoconf, make) for experimentation
- Reference compilers (gcc, clang) for feature checking

---

## Notes and Assumptions

### Assumptions
1. GMP library will handle arbitrary precision arithmetic portably
2. Modern compilers support computed goto (required for threaded code)
3. 64-bit pointers can be tagged similarly to 32-bit (if tagging is used)
4. Little-endian is acceptable (or endianness is already abstracted)
5. Original test suite is comprehensive enough to validate correctness

### Open Questions
*(To be answered during the study)*
1. What is the performance impact of 32→64 bit word size change?
2. Are there any hard-coded SPARC assembly sections in emulator?
3. How critical is register allocation optimization?
4. What is the impact of alignment strictness on ARM?
5. Are there any patents or licensing concerns?
6. Is the system actively maintained/used?

### Risks to Study Success
- **Incomplete Documentation:** Original design docs may be limited
- **Knowledge Loss:** Original developers may not be available
- **Hidden Dependencies:** Undocumented architecture assumptions
- **Scope Creep:** Study expanding into implementation
- **Obsolete Toolchain:** Build system may assume old tools

---

## Phase 1 Key Findings

### Critical Discovery: 64-bit Support Already Exists! ✅

The most significant finding from Phase 1 is that **AGENTS already has a complete 64-bit port for the DEC Alpha architecture**. This dramatically reduces porting risk for x86-64 and ARM64.

### Alpha 64-bit Port Characteristics

From `emulator/sysdeps.h` and `emulator/regdefs.h`:
- **TADBITS = 64:** Full 64-bit word size
- **PTR_ORG = 0:** No pointer adjustment needed
- **WORDALIGNMENT = 8:** 8-byte alignment
- **Small integers:** 58 bits (vs 26 on 32-bit)
- **Hard registers:** 10 registers allocated
- **Little-endian:** Matches x86-64/ARM64

### Implications for Modern Porting

1. **Tagged pointer scheme scales:** No redesign needed
2. **GC works on 64-bit:** Mark bit in high bit proven
3. **Memory alignment adapts:** WORDALIGNMENT macro handles it
4. **All data structures work:** Sizes double, but layouts compatible

### Platform Dependencies Cataloged

**8 existing platform ports analyzed:**
- Alpha (64-bit) - Primary reference
- SPARC, MIPS, HP-PA, AIX, Macintosh, Sequent, SVR4

**7 configuration flags documented:**
- THREADED_CODE, HARDREGS, BAM, TRACE, STRUCT_ENV, INDIRECTVARS, TADTURMS

**Complete file inventory:**
- 50 C source files
- 63 header files
- ~15,000 lines in engine.c + related files

### Recommended Minimal Changes for x86-64/ARM64

1. **sysdeps.h:** Add architecture detection
2. **regdefs.h:** Add register allocation (optional but recommended)
3. **Test PTR_ORG = 0:** Should work (like Alpha)
4. **Test alignment on ARM64:** May need OptionalWordAlign

### Confidence Level: HIGH ✅

Based on the Alpha port, porting to x86-64 and ARM64 should be **straightforward** with **low risk**.

---

## Change Log

| Date | Version | Author | Changes |
|------|---------|--------|---------|
| 2025-11-04 | 1.0 | Claude | Initial study plan created |
| 2025-11-04 | 1.1 | Claude | Phase 1 completed - all 3 architecture documents done |
| 2025-11-05 | 2.0 | Claude | **ALL STUDY PHASES COMPLETE** - All 10 documents finished |
| 2025-11-05 | 2.1 | Claude | Added Document 11 (Reality Check), rewrote Document 09 for Claude Code execution, defined implementation strategy |
| 2025-11-05 | 3.0 | Claude | **EXECUTION PLANNING COMPLETE** - Added Documents 12-13, created attempt infrastructure, ready for implementation |

---

## Completed Steps

1. ✅ **Create `porting-study` branch** - COMPLETED
2. ✅ **Write this STUDY-PLAN.md** - COMPLETED
3. ✅ **Create `docs/porting/` directory structure** - COMPLETED
4. ✅ **Complete Phase 1-5: All Study Documents** - COMPLETED (Documents 01-11)
5. ✅ **Complete Phase 6: Execution Planning** - COMPLETED
   - ✅ 12-OPTION-B-EXECUTION-PLAN.md (67 detailed steps)
   - ✅ 13-ITERATIVE-STRATEGY.md (Learning framework)
6. ✅ **Create attempt tracking infrastructure** - COMPLETED
   - ✅ docs/attempts/ directory structure
   - ✅ ATTEMPT-01 templates and planning documents
7. ✅ **Create implementation branch** - COMPLETED
   - ✅ Branch: `claude/port-attempt-01-011CUoHSMTFawkiKfVsuVuPP`

## Next Steps (When Implementation Begins)

**Status:** 🟡 Ready to begin - Awaiting go-ahead

**Note:** Per user request, implementation is NOT starting yet. Planning is complete and ready for execution.

**When implementation begins:**

1. **Execute ATTEMPT-01** following `12-OPTION-B-EXECUTION-PLAN.md`
   - Phase 1: Alpha Port Analysis (Steps 1.1-1.5)
   - Phase 2: Pattern Extraction (Steps 2.1-2.2)
   - Phase 3: x86-64 Configuration (Steps 3.1-3.4)
   - Phase 4: Incremental Implementation (Steps 4.1-4.5)
   - Phase 5: Build and Test (Steps 5.1-5.5)

2. **Document everything** in real-time
   - Update ATTEMPT-01-LOG.md during execution
   - Capture all errors, decisions, and learnings

3. **Analyze results** after attempt completes
   - Complete ATTEMPT-01-ANALYSIS.md
   - Complete ATTEMPT-01-RESULTS.md
   - Update LESSONS-LEARNED.md

4. **Iterate** based on learnings
   - Revise execution plan if needed
   - Create ATTEMPT-02 plan
   - Repeat until success

**Expected Timeline:** 3-5 attempts to achieve stable x86-64 build

---

## 🎉 PLANNING COMPLETE - Ready for Implementation 🎉

All 13 planning documents have been completed. The porting study and execution planning are now **COMPLETE**.

### Chosen Implementation Strategy: Option B (Direct Modern Port)

Based on comprehensive analysis in Documents 09 and 11, **Option B** has been selected as the primary approach.

**Option B: Direct Modern Port (Documents 11-12)**
- ✅ Leverage Alpha 64-bit port as proven blueprint
- ✅ Port directly to native x86-64 environment
- ✅ Skip vintage emulation complexity
- ✅ 67 detailed, verifiable steps defined
- ✅ Clear verification methods for each step
- **Success probability:** 60-70% autonomous, 90% with human backup

**Option A: Vintage Emulation (Document 09, Fallback)**
- Available as backup strategy if Option B encounters fundamental issues
- Requires more setup overhead (QEMU environment)
- **Success probability:** 70-80% with human collaboration for setup

### Execution Plan Overview (Document 12)

**Phase 1: Alpha Port Analysis** - Extract 64-bit porting patterns
**Phase 2: Pattern Extraction** - Create x86-64 mapping guide
**Phase 3: x86-64 Configuration** - Add platform support
**Phase 4: Incremental Implementation** - Build and fix iteratively
**Phase 5: Build and Test** - Validate with test suite
**Phase 6: ARM64 Adaptation** - Extend to ARM64

### Iterative Strategy (Document 13)

**Philosophy:** Expect failure, learn systematically, iterate to success

**Process:** PLAN → EXECUTE → DOCUMENT → ANALYZE → REVISE → REPEAT

**Infrastructure:**
- ✅ Attempt tracking structure created
- ✅ Documentation templates ready
- ✅ Cumulative knowledge framework established
- ✅ Branch created: `claude/port-attempt-01-011CUoHSMTFawkiKfVsuVuPP`

### Critical Insights from Document 11

**Claude Code CAN do:**
- ✅ All code analysis and modification (80-90% of work)
- ✅ Native x86-64 building and testing
- ✅ Systematic verification and documentation
- ✅ Iterative problem-solving with learning

**Claude Code CANNOT do alone:**
- ❌ Interactive QEMU setup (need human)
- ❌ Builds >10 minutes (need workarounds)
- ❌ Interactive debugging (need human escalation)

**Collaboration Model:** Claude Code leads implementation with human escalation for interactive tasks.

**Next Phase:** Execute ATTEMPT-01 following Document 12 guidance (when given go-ahead).

---

## Questions or Concerns?

Document any questions, blockers, or concerns here as they arise:

- **None yet** - Study just beginning

---

**End of Document**
