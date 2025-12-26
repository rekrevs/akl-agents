# Iterative Learning Strategy: Expect Failure, Learn, Improve

**Document 13 of 13**
**Version:** 1.0
**Date:** 2025-11-05
**Purpose:** Framework for learning from porting failures and systematically improving our approach

---

## Table of Contents

1. [Philosophy](#philosophy)
2. [Attempt Lifecycle](#attempt-lifecycle)
3. [Documentation Structure](#documentation-structure)
4. [Knowledge Extraction](#knowledge-extraction)
5. [Decision Framework](#decision-framework)
6. [Branch and Directory Strategy](#branch-and-directory-strategy)
7. [Success Metrics](#success-metrics)
8. [Long-Term Learning](#long-term-learning)

---

## 1. Philosophy

### 1.1 Core Principles

**Principle 1: Failure is Expected**
- First attempt will likely fail
- Failure provides information
- Each failure narrows the problem space
- No failure is wasted if documented

**Principle 2: Systematic Learning**
- Every error teaches us something
- Patterns emerge across failures
- Document the "why" not just the "what"
- Build institutional knowledge

**Principle 3: Incremental Progress**
- Small steps are better than big leaps
- Each attempt should reach further than the last
- Even failed attempts produce working code
- Progress is measured in understanding, not just working code

**Principle 4: Adaptation Over Perfection**
- Plans will be wrong - that's OK
- Adjust based on reality, not theory
- Be willing to pivot completely if needed
- The map is not the territory

---

### 1.2 What "Success" Looks Like

**After Attempt 1:**
- May not build completely, but we understand why
- Have documented 5-10 concrete issues
- Have revised plan based on real-world data
- Know what to try differently in Attempt 2

**After Attempt 3:**
- Should have complete build (even if tests fail)
- Should understand test failure patterns
- Should have refined approach significantly
- May be ready for production testing

**After Attempt 5:**
- Should have >90% test pass rate
- Should have production-ready build
- Should have comprehensive documentation
- Should be confident in the port

**Key Insight:** Each attempt doubles our knowledge.

---

## 2. Attempt Lifecycle

Each attempt follows this cycle:

```
PLAN → EXECUTE → DOCUMENT → ANALYZE → REVISE → PLAN (next attempt)
```

### 2.1 Phase 1: Plan

**Inputs:**
- Previous attempt results (if any)
- Updated understanding of the system
- Revised hypothesis

**Outputs:**
- `ATTEMPT-XX-PLAN.md` - What we will try
- Clear success criteria
- Stopping conditions

**Duration:** 1-2 hours (or one Claude session)

---

### 2.2 Phase 2: Execute

**Process:**
- Follow the plan step-by-step
- Document deviations in real-time
- Capture all errors verbatim
- Note unexpected behavior

**Rules:**
- Don't skip steps
- Don't "fix on the fly" without documenting
- Stop at stopping condition
- No heroics

**Duration:** 2-6 hours (or multiple Claude sessions)

---

### 2.3 Phase 3: Document

**Required Documentation:**

1. **What happened** - Chronological log
2. **What worked** - Successful steps
3. **What failed** - Errors and failures
4. **What surprised us** - Unexpected behavior
5. **What we learned** - New understanding

**File:** `ATTEMPT-XX-LOG.md`

**Format:**
```markdown
## Attempt Summary
- Started: 2025-11-05 14:30
- Ended: 2025-11-05 16:45
- Outcome: Partial success - build failed at step 4.3

## What Worked
- Phase 1: Alpha analysis complete (100%)
- Phase 2: Pattern extraction complete (100%)
- Phase 3: Configuration complete (100%)
- Phase 4: Build started successfully (50%)

## What Failed
- Step 4.3: First build failed with error:
  ```
  emulator/engine.c:234: error: impossible register constraint in 'asm'
  ```

## Root Cause Analysis
The x86-64 register constraint 'r15' conflicts with...

## Learnings
1. GCC version matters - 9.x vs 11.x handle constraints differently
2. Alpha register syntax doesn't translate directly
3. Need to test register constraints before full build

## Impact on Plan
- Step 3.2 needs revision: Add register constraint validation
- Step 4.3 needs new sub-step: Test minimal register constraint before full build
- May need fallback: Disable HARDREGS for first build attempt
```

**Duration:** 30 minutes

---

### 2.4 Phase 4: Analyze

**Questions to Answer:**

1. **Root Cause:** Why did the failure occur?
2. **Pattern:** Is this a one-off or systemic issue?
3. **Scope:** How many places are affected?
4. **Fix Effort:** How hard to fix? (easy/medium/hard/unknown)
5. **Alternatives:** What are other approaches?

**Output:** `ATTEMPT-XX-ANALYSIS.md`

**Template:**
```markdown
## Failure Analysis

### Issue #1: Register Constraint Error

**Symptom:** GCC error: impossible register constraint

**Root Cause:** x86-64 GCC requires specific syntax for hard register allocation

**Pattern:** Systemic - affects all register definitions in regdefs.h

**Scope:** ~8 register definitions

**Fix Difficulty:** Medium - need to research x86-64 GCC register allocation syntax

**Fix Estimate:** 1-2 hours

**Alternatives:**
1. Disable HARDREGS (fast, performance loss)
2. Fix syntax (medium, full performance)
3. Use different registers (medium, may work better)

**Recommendation:** Try fixing syntax first, fall back to disable if needed
```

**Duration:** 1 hour

---

### 2.5 Phase 5: Revise

**Action:** Update the execution plan based on analysis.

**What to Revise:**

1. **Add missing steps** - Based on what we learned
2. **Update verification** - Add checks we now know are needed
3. **Reorder steps** - If we learned a better sequence
4. **Add alternatives** - Document fallback options
5. **Update estimates** - Adjust difficulty/time based on reality

**Output:** Updated `12-OPTION-B-EXECUTION-PLAN.md` (version bump)

**Example Revision:**

**Old Step 3.2:**
```markdown
### Step 3.2: Add x86-64 Register Definitions

Add HARDREGS configuration for x86-64.

[Verification command]
```

**Revised Step 3.2:**
```markdown
### Step 3.2: Add x86-64 Register Definitions

Add HARDREGS configuration for x86-64.

**IMPORTANT:** Test register constraints BEFORE full build!

[New sub-step 3.2.1: Test Register Constraints]
[New sub-step 3.2.2: Validate with minimal program]
[Original verification command]
[NEW: Alternative if constraints fail - disable HARDREGS]
```

**Duration:** 30-60 minutes

---

### 2.6 Phase 6: Decide Next Steps

**Decision Tree:**

```
Did we make progress?
├─ YES: Continue with revised plan
│   └─ Start ATTEMPT-XX+1
├─ NO: Analyze why
    ├─ Wrong approach? → PIVOT
    │   └─ Major plan revision
    ├─ Missing knowledge? → RESEARCH
    │   └─ Deep dive into specific topic
    ├─ Tooling issue? → ESCALATE
    │   └─ Get human help
    └─ Need different strategy? → PIVOT
        └─ Consider Option A or hybrid
```

**Duration:** 15 minutes

---

## 3. Documentation Structure

### 3.1 Directory Layout

```
docs/
├── porting/
│   ├── 01-ARCHITECTURE-OVERVIEW.md
│   ├── ...
│   ├── 12-OPTION-B-EXECUTION-PLAN.md
│   └── 13-ITERATIVE-STRATEGY.md (this document)
└── attempts/
    ├── ATTEMPT-01/
    │   ├── ATTEMPT-01-PLAN.md
    │   ├── ATTEMPT-01-LOG.md
    │   ├── ATTEMPT-01-ANALYSIS.md
    │   ├── ATTEMPT-01-RESULTS.md
    │   ├── alpha-config-extracted.md
    │   ├── alpha-register-mapping.md
    │   ├── alpha-to-x86-64-mapping.md
    │   └── build.log
    ├── ATTEMPT-02/
    │   └── ...
    └── LESSONS-LEARNED.md (cumulative)
```

---

### 3.2 Document Templates

#### ATTEMPT-XX-PLAN.md

```markdown
# Attempt XX Plan

**Started:** YYYY-MM-DD
**Status:** planned|in-progress|completed|failed
**Expected Duration:** X hours
**Goal:** [One sentence goal]

## Changes from Previous Attempt

[What we're doing differently this time]

## Execution Plan

[Specific steps for this attempt - reference main plan]

## Success Criteria

- [ ] Criterion 1
- [ ] Criterion 2
- [ ] Criterion 3

## Stopping Conditions

[When to stop even if not complete]
```

---

#### ATTEMPT-XX-LOG.md

```markdown
# Attempt XX Log

**Started:** YYYY-MM-DD HH:MM
**Ended:** YYYY-MM-DD HH:MM
**Outcome:** [success|partial|failed]

## Chronological Log

### YYYY-MM-DD HH:MM - Phase 1 Start
[Details...]

### YYYY-MM-DD HH:MM - Step 1.1 Complete
[Details...]

### YYYY-MM-DD HH:MM - Error Encountered
```
Error: [exact error message]
File: [file path]
Line: [line number]
```
[Analysis...]

## Quick Summary

### Completed
- [x] Phase 1: Analysis
- [ ] Phase 2: Implementation

### Errors (in order of occurrence)
1. [Error 1 - resolved]
2. [Error 2 - blocked]

### Decisions Made
1. [Decision with rationale]
```

---

#### ATTEMPT-XX-ANALYSIS.md

```markdown
# Attempt XX Analysis

## Executive Summary

[2-3 sentences: What happened, why, what next]

## Detailed Failure Analysis

### Issue 1: [Title]

**Symptom:** ...
**Root Cause:** ...
**Pattern:** one-off|recurring|systemic
**Scope:** ...
**Fix Difficulty:** easy|medium|hard|unknown
**Fix Estimate:** ...
**Recommendation:** ...

[Repeat for each issue]

## Success Analysis

[What worked well? Why? Can we apply this elsewhere?]

## New Understanding

[What do we now know that we didn't before?]

## Impact on Overall Strategy

[Does this change our approach?]
```

---

#### ATTEMPT-XX-RESULTS.md

```markdown
# Attempt XX Results

## Quantitative Results

- **Build Success:** yes|no|partial
- **Files Modified:** X
- **Lines Changed:** X
- **Compilation Errors:** X
- **Compilation Warnings:** X
- **Tests Run:** X
- **Tests Passed:** X (Y%)
- **Tests Failed:** X

## Qualitative Results

**What Works:**
- [List of working functionality]

**What Doesn't Work:**
- [List of broken functionality]

**Partially Working:**
- [List with details]

## Artifacts Produced

- `build.log` - Full build output
- `test-results.log` - Test results
- `emulator/agents` - Binary (if produced)
- [Other files]

## Knowledge Artifacts

- New understanding of [topic]
- Confirmed hypothesis about [thing]
- Disproved assumption about [thing]
```

---

### 3.3 Cumulative Knowledge: LESSONS-LEARNED.md

This file accumulates knowledge across ALL attempts.

```markdown
# Cumulative Lessons Learned

## Lesson 1: GCC Register Constraints on x86-64

**Discovered In:** ATTEMPT-01
**Issue:** GCC rejects certain register constraint syntax
**Solution:** Use proper GCC syntax: `asm("r15")` not `asm("%r15")`
**Applies To:** All hard register allocations
**Confidence:** High (tested)

## Lesson 2: Build System Quirks

**Discovered In:** ATTEMPT-01
**Issue:** Autoconf 1.x doesn't run on modern systems
**Solution:** Manually edit `configure` script
**Applies To:** Initial setup
**Confidence:** Medium (workaround)

[etc...]
```

---

## 4. Knowledge Extraction

### 4.1 What to Extract

After each attempt, extract:

1. **Technical Facts**
   - "x86-64 requires 8-byte alignment"
   - "GCC 11 handles register constraints differently than GCC 9"

2. **Patterns**
   - "All pointer manipulations need TADBITS review"
   - "Test failures cluster around memory operations"

3. **Process Insights**
   - "Testing register constraints separately saves hours"
   - "Building in verbose mode reveals hidden warnings"

4. **Assumptions Validated/Invalidated**
   - "✓ Alpha port is indeed a good blueprint"
   - "✗ x86-64 register mapping is NOT straightforward"

5. **Risks Materialized**
   - "Register allocation was marked MEDIUM risk - now HIGH"
   - "Build system was marked HIGH risk - confirmed"

---

### 4.2 How to Extract

**Process:**

1. **Review logs** - Read chronologically
2. **Identify patterns** - Look for repeated issues
3. **Distill lessons** - Abstract from specifics
4. **Rate confidence** - How sure are we?
5. **Document implications** - What does this mean for future steps?

**Template:**

```markdown
## Lesson: [Title]

**Context:** [When/where discovered]
**Observation:** [What we saw]
**Explanation:** [Why it happened]
**Implication:** [What it means]
**Confidence:** [Low/Medium/High]
**Applied In:** [Where we used this knowledge]
```

---

## 5. Decision Framework

### 5.1 When to Continue vs Pivot

**Continue with Current Approach When:**
- Making steady progress (each attempt reaches further)
- Failures are fixable with reasonable effort
- Understanding is increasing
- No fundamental blockers

**Pivot to Alternative Approach When:**
- Stuck at same point for 3+ attempts
- Fixes create more problems than they solve
- Discovered fundamental incompatibility
- Better alternative becomes apparent

**Escalate to Human When:**
- Need interactive debugging (GDB)
- Need to research obscure topics >2 hours
- Need to make strategic decision
- Completely stuck

---

### 5.2 Quantitative Metrics

Track these across attempts:

| Metric | Attempt 1 | Attempt 2 | Attempt 3 | Target |
|--------|-----------|-----------|-----------|--------|
| Files Modified | X | X | X | ~15 |
| Compilation Errors | X | X | X | 0 |
| Compilation Warnings | X | X | X | <10 |
| Link Errors | X | X | X | 0 |
| Tests Run | X | X | X | 60 |
| Tests Passed | X | X | X | 57 (95%) |
| Furthest Phase Reached | X | X | X | Phase 7 |

**Progress Indicators:**

- **Compilation errors decreasing:** Good
- **Warnings increasing:** May indicate deeper issues being exposed
- **Tests passing increasing:** Great
- **Furthest phase reached increasing:** Excellent

**Red Flags:**

- **Same error 3 attempts in a row:** Wrong approach
- **New errors appearing faster than fixes:** Systemic issue
- **Test pass rate decreasing:** Regressions

---

### 5.3 Qualitative Assessment

After each attempt, rate:

| Dimension | Rating (1-5) | Notes |
|-----------|--------------|-------|
| Understanding | | How well do we understand the system? |
| Progress | | Are we moving forward? |
| Code Quality | | Is our code clean and maintainable? |
| Confidence | | How confident are we in success? |
| Morale | | Is this still tractable? |

**1 = Poor, 3 = Acceptable, 5 = Excellent**

---

## 6. Branch and Directory Strategy

### 6.1 Branch Naming

**Pattern:** `claude/port-attempt-XX`

**Examples:**
- `claude/port-attempt-01` - First attempt
- `claude/port-attempt-02` - Second attempt (after learning)
- `claude/port-attempt-03-alternative` - Testing different approach

**Rules:**
- One branch per attempt
- Never force-push
- Never delete attempt branches (keep for reference)
- Merge successful attempt to main branch

---

### 6.2 Commit Strategy

**During Attempt:**

Commit frequently with descriptive messages:

```
Phase 1.1 complete: Alpha analysis extracted
Phase 3.1: Add x86-64 platform detection
Fix: Register constraint syntax for x86-64
WIP: Debugging build error in engine.c
```

**After Attempt:**

Tag the final state:

```bash
git tag -a attempt-01-final -m "Attempt 01 final state: Build failed at step 4.3"
```

---

### 6.3 Directory Organization

**Source Code:**
- Stays in `emulator/`, `tests/`, etc.
- Changes tracked by git

**Documentation:**
- `docs/porting/` - Strategy documents (shared across attempts)
- `docs/attempts/ATTEMPT-XX/` - Per-attempt documentation

**Build Artifacts:**
- `.gitignore` these (too large)
- Save key files (build.log, test-results.log) in docs/attempts/

---

## 7. Success Metrics

### 7.1 Per-Attempt Success

**Minimum Viable Attempt:**
- Completed at least one full phase
- Documented all errors encountered
- Extracted at least 3 lessons
- Updated plan for next attempt

**Good Attempt:**
- Completed 2+ phases
- Fixed at least 5 issues
- No regressions from previous attempt
- Clear path forward

**Excellent Attempt:**
- Reached new furthest point
- All previous issues still fixed
- Produced working artifacts
- High confidence in next steps

---

### 7.2 Overall Success

**Minimum Success (First Successful Build):**
- [x] Compiles without errors on x86-64
- [x] Produces executable
- [x] Executable runs without segfault
- [ ] Passes >50% of tests

**Target Success (Production Ready):**
- [x] Compiles cleanly (no errors, <10 warnings)
- [x] Passes >95% of tests
- [x] Performance within 10% of native SPARC
- [x] Documented quirks and limitations

**Stretch Success (Perfect Port):**
- [x] 100% test pass
- [x] Full HARDREGS support
- [x] Optimized for modern CPUs
- [x] Also works on ARM64

---

### 7.3 Knowledge Success

**Even if the port fails, we succeed if:**
- Comprehensive understanding of why it failed
- Documented all blockers clearly
- Provided roadmap for human to complete
- Extracted reusable knowledge

**Key Insight:** Understanding failure is more valuable than accidental success.

---

## 8. Long-Term Learning

### 8.1 Meta-Learning

After 3-5 attempts, review the process itself:

**Questions:**
1. Is our documentation useful?
2. Are we extracting the right lessons?
3. Is our decision framework working?
4. What would we do differently if starting over?

**Output:** Update this document (13-ITERATIVE-STRATEGY.md)

---

### 8.2 Knowledge Transfer

This porting experience teaches us about:
- Working with legacy C code
- Cross-architecture porting
- Build system archaeology
- Systematic problem solving
- Iterative development

**Document for future projects:**
- What worked in this process
- What didn't work
- Lessons applicable beyond this project

---

### 8.3 Closing the Loop

**When the port is complete:**

1. Write final retrospective
2. Update all strategy documents with lessons learned
3. Create "guide for next porter" document
4. Archive attempt branches
5. Celebrate success (or document noble failure)

---

## Summary

**Core Message:** Expect to fail, plan to learn, iterate to succeed.

**Key Practices:**
1. Document everything in real-time
2. Extract lessons after each attempt
3. Update plans based on reality
4. Measure progress quantitatively and qualitatively
5. Know when to pivot vs persevere

**Remember:**
- First attempt will likely fail - that's OK
- Each attempt teaches us more than the last
- Documented failure is better than undocumented success
- The goal is a working port AND deep understanding

**Next Steps:**
- Set up attempt infrastructure
- Create branch for ATTEMPT-01
- Begin execution of 12-OPTION-B-EXECUTION-PLAN.md
- Document everything in real-time

---

**Related Documents:**
- **12-OPTION-B-EXECUTION-PLAN.md** - What to do
- **11-CLAUDE-CODE-REALITY-CHECK.md** - Why this approach
- **STUDY-PLAN.md** - Overall project tracker
