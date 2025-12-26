# Consistency Issues in Draft Outline Documents

**Date**: November 7, 2025
**Files Examined**: 00-MASTER-OUTLINE.md, OUTLINE.md, README.md

---

## Issue 1: Outdated 00-MASTER-OUTLINE.md

**File**: `00-MASTER-OUTLINE.md`
**Status**: **OUTDATED** - Does not match current draft state

**Problems**:

1. **Lines 29-32**: Claims thesis is "not yet available"
   ```
   **To obtain:**
   - Sverker Janson's thesis on AKL (mentioned but not yet available)
   ```
   **Reality**: We HAVE the thesis (docs/thesis-parts/thesis-part-01.md through 23)

2. **Structure mismatch**: Only describes Parts I-II (8 chapters)
   **Reality**: Draft has 18 chapters complete, OUTLINE.md describes 25 chapters

3. **Date**: November 6, 2025
   **Reality**: Validation work completed November 7, 2025

**Recommendation**: Either update 00-MASTER-OUTLINE.md or remove it (superseded by OUTLINE.md)

---

## Issue 2: README.md Needs Date Update

**File**: `README.md`
**Line 370**: States "Date: November 6, 2025"
**Current date**: November 7, 2025

**Also needs update for validation work**:
- Port semantics corrected
- Guard operators fully documented
- Chapter 13 & operator error fixed

---

## Issue 3: README.md Branch Information

**Line 371**: States branch `claude/draft-inside-agents-book-011CUsNSbHYNJMH8ApVc7GDt`
**Current branch**: `claude/thesis-review-011CUtkv2EJhgbVJtVJsFh6o`

The README is from a different branch/session.

---

## Positive Findings: AKL Features Correct ✅

After all corrections, both OUTLINE.md and README.md correctly describe AKL:

**README.md Line 30**: ✅ Correct
```
- **Three types of guards** (conditional `→`, committed choice `|`, nondeterminate `?`)
```

**README.md Lines 174-179**: ✅ Correct
```
The **three guard operators** (`→`, `|`, `?`) are the key innovation. AKL uniquely combines:
- **Conditional choice** (`→`): deterministic execution
- **Committed choice** (`|`): don't-care nondeterminism (reactive programming)
- **Nondeterminate choice** (`?`): don't-know nondeterminism (search with backtracking)
```

**No mention of incorrect `&` operator** ✅

**Ports correctly described** ✅

---

## Recommendations

1. **Remove or update 00-MASTER-OUTLINE.md**
   - It's superseded by OUTLINE.md
   - Contains outdated information
   - May confuse readers

2. **Update README.md**
   - Change date to November 7, 2025
   - Add note about validation work completed
   - Update branch reference if needed

3. **Keep OUTLINE.md as-is**
   - It's the authoritative outline
   - Correctly structured
   - Matches actual draft chapters

---

## Summary

**Semantic correctness**: ✅ Both OUTLINE.md and README.md correctly describe AKL features (after fixes)

**Structural consistency**: ⚠️ 00-MASTER-OUTLINE.md is outdated and inconsistent with actual state

**Metadata**: ⚠️ README.md date and branch info need updating
