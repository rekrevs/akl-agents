# CRITICAL: Missing Guard Semantics in Draft

## Problem

The draft chapters (including the corrected Chapter 2) **do not properly explain** that AKL has **THREE different types of choice statements** with **THREE different guard operators** and **THREE fundamentally different semantics**.

This is a **critical omission** from Janson's thesis specification.

---

## The Three Types of Choice Statements

From Janson's thesis (Sections 2.3, 2.4, 2.5):

### 1. Conditional Choice - `→` (then)
**Section 2.3** - **Deterministic choice**

**Syntax**:
```
( Guard → Body
; Guard → Body
; ... )
```

**Semantics**:
- If guard is **entailed** → take that branch
- If guard is **disentailed** → try next clause
- If neither (unknown) → **wait/suspend**
- Used for: **deterministic conditional execution**

**Example from thesis**:
```
append(X, Y, Z) :=
  ( X = [] → Z = Y
  ; X = [E|X1] → append(X1, Y, Z1), Z = [E|Z1] ).
```

---

### 2. Committed Choice - `|` (commit)
**Section 2.4: DON'T CARE NONDETERMINISM**

**Syntax**:
```
( Guard | Body
; Guard | Body
; ... )
```

**Semantics**:
- Ask **all guards** from the store
- If **any** guard is entailed → **commit to that branch** (arbitrary choice)
- If guard is disentailed → delete that clause
- If no clauses remain → **fail**
- Otherwise → **wait**
- Used for: **reactive programming** (react to first available input)

**Example from thesis**:
```
merge(X, Y, Z) :=
  ( X = [] | Z = Y
  ; Y = [] | Z = X
  ; E, X1 : X = [E|X1] | Z = [E|Z1], merge(X1, Y, Z1)
  ; E, Y1 : Y = [E|Y1] | Z = [E|Z1], merge(X, Y1, Z1) ).
```

The merge agent can react as soon as **either** X or Y is available. It **doesn't matter** which one arrives first - hence "don't care".

---

### 3. Nondeterminate Choice - `?` (wait)
**Section 2.5: DON'T KNOW NONDETERMINISM**

**Syntax**:
```
( Guard ? Body
; Guard ? Body
; ... )
```

**Semantics**:
- Ask **all guards** from the store
- If guard is disentailed → delete that clause
- If **only one clause** remains (determinate) → commit to it
- If **multiple clauses** remain → **wait for more information**
- If no more computation possible with multiple clauses → **try each in different copies** (backtracking/search)
- Used for: **search**, **generate-and-test**, **logic programming**

**Example from thesis**:
```
p(X) :=
  ( X = a ? true
  ; X = b ? true ).
```

When called with unconstrained X, this will **search** and produce two alternative results: X=a and X=b.

Another example:
```
member(E, L) :=
  ( L = [E|_] ? true
  ; L = [_|L1] ? member(E, L1) ).
```

This can **generate** all members of a list, or **test** membership.

---

## Deep Guards vs Flat Guards

**All three types** of choice statements support both:

### Flat Guards (Section 2.3-2.5)
Guards contain **only constraints**

Example:
```
( X = [] → Z = Y    % Flat guard: just a constraint
; ... )
```

### Deep Guards (Section 2.6)
Guards contain **arbitrary statements** including recursive calls

From thesis Section 2.6:

> "Although we have ignored it up to this point, any statement may be used as a guard in a choice statement. The behaviour presented above has been that of the special case when conditions and guards are constraints. Such guards are often referred to as *flat*. This will now be generalised to *deep guards*, which contain arbitrary statements."

Example with deep guard:
```
( append(X, Y, Z) → p(Z)    % Deep guard: contains agent call
; true → q(X, Y) )
```

When the guard contains statements, they are executed **locally** in a separate constraint store. The guard succeeds when the local computation terminates successfully, and the asked constraint is the conjunction of constraints in the local store.

---

## What Draft Chapter 2 Currently Says

The current Chapter 2 (after my corrections) **conflates all three types** and doesn't clearly distinguish them:

### Lines 197-213: Mentions `?` operator
```
The `?` operator marks **commitment**:

clause1(X) := guard1 ? body1.
clause2(X) := guard2 ? body2.
```

But this is **WRONG** - the `?` operator is for **nondeterminate choice** (don't know nondeterminism), NOT for general "commitment"!

### Lines 317-323: No mention of `|` operator
The section on "Don't Care Nondeterminism" doesn't even exist!

### No clear distinction
The three operators `→`, `|`, and `?` are not clearly explained as having fundamentally different semantics.

---

## Impact

**CRITICAL**: Anyone learning AKL from the draft will:
- ❌ Not understand the three different types of nondeterminism
- ❌ Not know when to use `→` vs `|` vs `?`
- ❌ Not understand don't care vs don't know nondeterminism
- ❌ Not understand the difference between reactive programming (`|`) and search (`?`)

This is **fundamental to AKL's design** and distinguishes it from both:
- Prolog (which only has don't know nondeterminism with `?`)
- GHC/concurrent logic languages (which only have don't care nondeterminism with `|`)

**AKL uniquely combines all three** in a coherent framework.

---

## Required Corrections to Chapter 2

### Section 1: Basic Conditional Choice (`→`)
**New subsection needed**: "Conditional Choice with →"

Explain:
- Deterministic choice based on entailment
- Wait if neither entailed nor disentailed
- Examples: append, list processing

### Section 2: Committed Choice (`|`)
**New section needed**: "Don't Care Nondeterminism with |"

Explain:
- Reactive programming
- React to first available input
- Arbitrary choice among entailed guards
- Examples: merge, process communication

### Section 3: Nondeterminate Choice (`?`)
**Rewrite existing section**: Currently calls it "commitment" which is wrong

Explain:
- Don't know nondeterminism
- Search and backtracking
- Wait for determinacy
- Try alternatives in separate computation copies
- Examples: member, search problems

### Section 4: Deep Guards
**Add new section**: "General Statements in Guards"

Explain:
- Flat vs deep guards
- Local computation in guards
- Local constraint stores
- Examples with agent calls in guards

---

## Correct Examples

### Example 1: Conditional Choice `→`
```
% Deterministic: wait for information
append(X, Y, Z) :=
  ( X = [] → Z = Y
  ; X = [E|X1] → append(X1, Y, Z1), Z = [E|Z1] ).
```

### Example 2: Committed Choice `|`
```
% Don't care: react to first available
merge(X, Y, Z) :=
  ( X = [] | Z = Y
  ; Y = [] | Z = X
  ; E, X1 : X = [E|X1] | Z = [E|Z1], merge(X1, Y, Z1)
  ; E, Y1 : Y = [E|Y1] | Z = [E|Z1], merge(X, Y1, Z1) ).
```

### Example 3: Nondeterminate Choice `?`
```
% Don't know: search for solutions
member(E, L) :=
  ( L = [E|_] ? true
  ; L = [_|L1] ? member(E, L1) ).
```

### Example 4: Deep Guard
```
% Guard contains arbitrary computation
choose_solution(X, Y, Result) :=
  ( some_complex_check(X) → Result = solution1(X)
  ; another_check(Y) → Result = solution2(Y)
  ; Result = default ).
```

---

## Summary Table

| Operator | Type | Nondeterminism | Use Case | Wait Condition |
|----------|------|----------------|----------|----------------|
| `→` | Conditional | None (deterministic) | Conditional execution | Unknown (not entailed/disentailed) |
| `|` | Committed | Don't care | Reactive programming | No entailed guards |
| `?` | Nondeterminate | Don't know | Search, logic programming | Multiple clauses remain |

| Guard Type | Contents | Execution |
|------------|----------|-----------|
| Flat | Constraints only | Direct ask from store |
| Deep | Arbitrary statements | Local computation with local store |

---

## References

- **Janson's Thesis Section 2.3**: Conditional choice with `→`
- **Janson's Thesis Section 2.4**: Committed choice with `|` (DON'T CARE)
- **Janson's Thesis Section 2.5**: Nondeterminate choice with `?` (DON'T KNOW)
- **Janson's Thesis Section 2.6**: General statements in guards (deep guards)

---

**Document created**: November 7, 2025
**Priority**: **CRITICAL** - Chapter 2 needs major rewrite
**Impact**: Fundamental misrepresentation of AKL's core semantics
