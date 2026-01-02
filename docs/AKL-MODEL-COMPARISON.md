# AKL Computation Model Comparison

**Date:** 2026-01-02
**Purpose:** Compare the provided formal AKL computation model with the AGENTS implementation and documentation

---

## Executive Summary

This document compares three perspectives on AKL computation:

1. **Provided Formal Model** - A detailed operational semantics with explicit rules for and-boxes, choice-boxes, unification, guards, and incremental choice splitting
2. **AGENTS Implementation** - The C-based emulator in `emulator/` (~113 files, 3.3MB)
3. **AGENTS Documentation** - The formal semantics in `doc/internals.tex` (2765 lines)

### Key Findings

✅ **Strong Alignment**: Core concepts (and-boxes, choice-boxes, guards, unification, promotion) are present in all three

⚠️ **Semantic Differences**: Several important differences in guard operators, splitting mechanisms, and configuration representation

❌ **Missing Features**: The provided model's incremental ?-splitting and explicit SYNC rule are not directly implemented

---

## Table of Contents

1. [Core Objects and Representation](#1-core-objects-and-representation)
2. [Unification Mechanism](#2-unification-mechanism)
3. [Guard Semantics](#3-guard-semantics)
4. [Choice Promotion Rules](#4-choice-promotion-rules)
5. [Splitting Mechanism](#5-splitting-mechanism)
6. [Store Management](#6-store-management)
7. [Key Discrepancies](#7-key-discrepancies)
8. [Alignment Opportunities](#8-alignment-opportunities)

---

## 1. Core Objects and Representation

### 1.1 And-Boxes

| Aspect | Provided Model | Implementation | Documentation |
|--------|----------------|----------------|---------------|
| **Structure** | `and(Goals; s; V)` | `struct andbox` | `AND(Goals)_V` |
| **Goals** | Sequence of goals | Implicit in continuations | Sequence of atoms |
| **Store** | Explicit store `s: {X ↦ t}` | `unifier *unify` + `env` | Constraints list |
| **Locals** | Set `V` of local variables | `envid *env` (pointer) | Set of variables |
| **Status** | Implicit (solvedness) | `sflag status` (DEAD/STABLE/UNSTABLE) | Alive/Dead |

**Implementation Details** (`emulator/tree.h:66-80`):
```c
typedef struct andbox {
  sflag          status;        // DEAD, STABLE, UNSTABLE, XSTABLE, XUNSTABLE
  struct envid   *env;          // Environment pointer
  struct unifier *unify;        // Unification constraints
  struct constraint *constr;    // Other constraints
  struct choicebox *tried;      // Choice-boxes in this and-box
  struct andcont *cont;         // Continuation stack
  struct choicebox *father;     // Parent choice-box
  struct andbox *next, *previous; // Linked list of alternatives
} andbox;
```

**Analysis:**
- ✅ Both have goals, stores, and local variable tracking
- ⚠️ Implementation uses implicit goal representation (continuations/code pointers)
- ⚠️ Provided model has explicit store `s`, implementation distributes this across `unify`, `constr`, and variable bindings
- ✅ Both track parent/child relationships

### 1.2 Choice-Boxes

| Aspect | Provided Model | Implementation | Documentation |
|--------|----------------|----------------|---------------|
| **Structure** | `choice_op(K1; ...; Km)` | `struct choicebox` | `CHOICE(G1, ..., Gn)` |
| **Operator** | `op ∈ {!, j, ?}` | Implicit in guard instruction | `wait`, `cut`, `commit` |
| **Clauses** | `Ki = and(...) op (Body)` | `andbox *tried` | Guarded goals |
| **Definition** | Stored per clause | `predicate *def` | Sequence of clauses |

**Implementation Details** (`emulator/tree.h:95-107`):
```c
typedef struct choicebox {
  struct choicecont *cont;      // Choice continuation (label, args)
  struct choicebox *next, *previous;
  struct andbox *father;        // Parent and-box
  struct predicate *def;        // Predicate definition
  struct andbox *tried;         // And-boxes (alternative clauses)
  int type;                     // ZIPPEDIDOO=17 for special handling
} choicebox;
```

**Analysis:**
- ✅ Both represent choice points with multiple alternatives
- ⚠️ Operator semantics differ (see Section 3)
- ⚠️ Implementation doesn't explicitly distinguish `!`, `j`, `?` operators at choice-box level
- ✅ Both track tried alternatives as linked list of and-boxes

### 1.3 Global Configuration

| Aspect | Provided Model | Implementation | Documentation |
|--------|----------------|----------------|---------------|
| **Top-Level** | Multiset `{A1, ..., Ak}` | Single tree rooted at `choicebox *root` | Tree structure |
| **Branching** | Flat multiset of and-boxes | Hierarchical tree | Hierarchical tree |
| **Or-Boxes** | None (eliminated by design) | None | `OR(G1, ..., Gn)` |

**Analysis:**
- ❌ **Major Difference**: Provided model uses flat multiset, implementation uses tree
- ⚠️ Documentation has OR-boxes created by non-determinate promotion
- ✅ Provided model explicitly removes need for "top choice-box wrapper"

---

## 2. Unification Mechanism

### 2.1 Unification Rules

The provided model specifies explicit unification rules **U0-U7**:

| Rule | Description | Implementation Location |
|------|-------------|------------------------|
| **U0** | Normalize equation by stores | `unify.c` (implicit dereferencing) |
| **U1** | Delete `t=t` | `unify.c:unify()` |
| **U2** | Decompose `f(...)=f(...)` | `unify.c` (structure unification) |
| **U3** | Clash detection | `unify.c` (type mismatch) |
| **U4** | Orient `t=X` → `X=t` | `unify.c` (implicit) |
| **U5** | Occurs check | `unify.c:occurs()` |
| **U6** | Bind fresh variable | `unify.c` + trail |
| **U7** | Reduce via binding | `unify.c` (dereferencing) |

**Implementation Details** (`emulator/unify.c:108-351`):
```c
bool unify(x, y, andb, exs)
     andbox *andb;
     Term x, y;
     exstate *exs;
{
  // Handles rational tree unification
  // Uses hash table for cycle detection
  // Distinguishes local vs external variables
  // Trails bindings for backtracking
}
```

**Analysis:**
- ✅ All rules U0-U7 are implemented, though not explicitly separated
- ✅ Rational tree unification (handles cycles)
- ✅ Occurs check implemented
- ⚠️ Trail mechanism not explicit in provided model (but necessary for backtracking)

### 2.2 SYNC Rule (Obligatory External Match)

**Provided Model Specification:**
```
SYNC (mandatory match when both bind the same external variable)
If X ∉ Vg, X ∈ dom(sg) and X ∈ dom(S):
  let tg = sg(X)↓(sg,S)
  let te = S(X)↓(sg,S)
  Rewrite: and(Ḡ; sg; Vg) → and((tg=te), Ḡ ; sg \ {X↦sg(X)} ; Vg)
```

**Implementation:**
- ⚠️ **Partial**: Environment synchronization exists but works differently
- ✅ `install()` function checks constraint compatibility (`emulator/tree.c`)
- ❌ No explicit decomposition of external variable bindings into local equations
- ⚠️ Instead: fails immediately if incompatible (Environment Synchronization rule)

**Documentation** (`doc/internals.tex:2464`):
```
Environment Synchronization:
  AND(R)_V ⇒ FAIL if σ(R) is incompatible with environment
```

**Analysis:**
- ❌ **Missing**: Provided model's incremental SYNC decomposition not implemented
- ⚠️ Implementation uses coarser-grained compatibility checking
- ⚠️ This affects when guards can become quiet

---

## 3. Guard Semantics

### 3.1 Guard Operators

| Operator | Provided Model | Implementation | Documentation | Semantics Match? |
|----------|----------------|----------------|---------------|------------------|
| **!** | Ordered, requires solved+quiet | `GUARD_UNIT` + leftmost | `cut` | ⚠️ Partial |
| **j** | Any solved+quiet clause | `GUARD_UNORDER` | `commit` | ⚠️ Different |
| **?** | Deterministic when 1 clause, split otherwise | None directly | `wait` | ❌ Missing |

**Implementation Details** (`emulator/engine.c:374-523`):

```c
// GUARD_UNIT (maps to ! and cut in documentation)
CaseLabel(GUARD_UNIT):
  if(Solved(andb) && Quiet(andb) && EmptyTrail(exs)) {
    if(Leftmost(andb)) {
      // Promote and kill all other alternatives
      KillAll(chb);
      // ... promotion code ...
    }
  }

// GUARD_ORDER (ordered disjunction)
CaseLabel(GUARD_ORDER):
  if(Quiet(andb) && EmptyTrail(exs) && Solved(andb) && Leftmost(andb)) {
    // Promote, don't kill alternatives
    // ... promotion code ...
  }

// GUARD_UNORDER (commit - any quiet+solved can commit)
CaseLabel(GUARD_UNORDER):
  if(Quiet(andb) && EmptyTrail(exs) && Solved(andb)) {
    // Promote without leftmost requirement
    // ... promotion code ...
  }
```

### 3.2 Solvedness and Quietness

**Provided Model:**
```
Solved(and((); s; V)) - guard has no remaining goals
Quiet(and(Ḡ; s; V)) iff dom(s) ⊆ V - no external variable bindings
```

**Implementation:**
```c
#define Solved(A)        ((A)->cont->next == NULL)
#define Quiet(A)         (IsEmpty((A)->constr) && IsEmpty((A)->unify))
```

**Documentation** (`doc/internals.tex:2413`):
```
C_V is quiet if σ(C) does not restrict the environment outside V
```

**Analysis:**
- ✅ Solvedness matches: no remaining goals
- ⚠️ Quietness differs:
  - Provided: no external vars in store domain
  - Implementation: no constraints in lists
  - Documentation: constraints don't restrict environment
- ⚠️ Implementation's definition is more practical but less precise

### 3.3 Leftmost Predicate

**Provided Model:** Not explicitly required for all operators

**Implementation:** Required for `GUARD_UNIT` and `GUARD_ORDER`

```c
#define Leftmost(A)  ((A)->previous == NULL)
```

**Documentation:** Leftmost candidate should be chosen for non-determinate promotion

**Analysis:**
- ⚠️ Implementation enforces leftmost for deterministic guards (! and ordered)
- ✅ Matches Andorra principle (determinate first)
- ⚠️ Provided model's `j` operator doesn't require leftmost, implementation's `GUARD_UNORDER` doesn't either

---

## 4. Choice Promotion Rules

### 4.1 Promotion Conditions

| Rule | Provided Model | Implementation | Match? |
|------|----------------|----------------|--------|
| **CH-!** | First clause solved+quiet | `GUARD_UNIT` with leftmost | ⚠️ Partial |
| **CH-j** | Any clause solved+quiet | `GUARD_UNORDER` | ✅ Yes |
| **CH-?** | Single clause solved (no quiet requirement) | None | ❌ Missing |

**Provided Model Rules:**

```
CH-! (ordered, requires solved+quiet for first surviving clause)
  and(Pre, choice!(K1; Krest), Post; s; V)
  → and(Pre, Bindings(s1), B̄1, Post; s; V ∪ V1)
  if K1 is first AND Solved(guard1) AND Quiet(guard1)

CH-j (any solved+quiet clause)
  and(Pre, choicej(...; Ki; ...), Post; s; V)
  → and(Pre, Bindings(si), B̄i, Post; s; V ∪ Vi)
  if Solved(guardi) AND Quiet(guardi)

CH-? (deterministic when single clause)
  and(Pre, choice?(K), Post; s; V)
  → and(Pre, Bindings(sg), B̄, Post; s; V ∪ Vg)
  if ONLY ONE clause AND Solved(guard) (NO quietness requirement)
```

**Implementation:**
- ✅ `GUARD_UNIT` approximates CH-!
- ✅ `GUARD_UNORDER` approximates CH-j
- ❌ No CH-? equivalent (wait-guards use different mechanism)

**Documentation** (`doc/internals.tex:2472-2490`):
```
Determinate Promotion:
  AND(R, CHOICE(C_V % A), S)_W ⇒ AND(R, C, A, S)_{V ∪ W}
  if C_V is satisfiable and (if % is cut or commit) quiet

Cut:
  CHOICE(R, C_V ! A, S) ⇒ CHOICE(R, C_V ! A)
  if S non-empty and C_V satisfiable and quiet

Commit:
  CHOICE(R, C_V | A, S) ⇒ CHOICE(C_V | A)
  if R or S non-empty and C_V satisfiable and quiet
```

**Analysis:**
- ⚠️ Documentation uses 3 guard operators (wait `:`, cut `!`, commit `|`)
- ⚠️ Implementation uses 3 guard types (UNIT, ORDER, UNORDER)
- ⚠️ Mapping is not 1:1:
  - `GUARD_UNIT` ≈ cut (but kills all alternatives)
  - `GUARD_ORDER` ≈ ordered disjunction (not in docs)
  - `GUARD_UNORDER` ≈ commit
- ❌ Wait guards (`:`) don't have deterministic promotion in provided model

---

## 5. Splitting Mechanism

### 5.1 Split Rules

**Provided Model:**
```
SPLIT-? (copy one solved-guard clause)
  {…, A, …} where A = and(Pre, choice?(K1;…;Km), Post; s; V)
  and Stable({…,A,…})
  and Ki has Solved(guard_i)

  → { …, A_rest, …, ρ(A_promote), … }

  where:
    A_promote = and(Pre, Bindings(si), B̄i, Post; s; V ∪ Vi)
    A_rest = and(Pre, choice?(K1;…;K_{i-1}; K_{i+1};…;Km), Post; s; V)
    ρ = fresh renaming of locals
```

**Key Properties:**
- Only for `?` operator
- Only when stable
- One solved clause per split
- Explicit α-renaming
- Creates two configurations in multiset

**Implementation** (`emulator/engine.c:3276-3340`, `emulator/copy.c:1-1531`):

```c
splithandler:
  fork = cand->father;           // Choice-box containing candidate
  mother = fork->father;         // And-box containing choice-box
  root = mother->father;         // Choice-box above mother

  promoted = copy(cand, exs);    // Deep copy with shared structure

  if(fork->tried->next == NULL) {
    Promote(exs, fork->tried);
  } else {
    if(Stable(mother)) {
      Split(exs, mother);
    }
  }
```

**Documentation** (`doc/internals.tex:2492-2502`):
```
Non-Determinate Promotion:
  AND(T1, CHOICE(R, C_V : A, S), T2)_W
  ⇒ OR(AND(T1, C, A, T2)_{V∪W}, AND(T1, CHOICE(R, S), T2)_W)
  if R or S non-empty and stability conditions met
```

**Analysis:**
- ❌ **Major Difference**:
  - Provided model: incremental split (one clause at a time)
  - Implementation: creates copy, but not explicit multiset
  - Documentation: creates OR-box with both alternatives
- ❌ Provided model eliminates OR-boxes entirely
- ⚠️ Implementation uses `copy()` which does α-renaming implicitly
- ⚠️ Implementation tracks alternatives as tree, not multiset

### 5.2 Candidate Selection

**Provided Model:**
- Leftmost eligible `?` choice-box
- Must be stable
- Guard must be solved

**Implementation** (`emulator/candidate.c:51-194`):
```c
andbox *candidate(choicebox *chb) {
  return leftmost(chb);
}

andbox *leftmost(choicebox *chb) {
  // Finds leftmost GUARD_WAIT with no children
  // Or GUARD_CUT if leftmost and no children
  // Recurses into children depth-first
}
```

**Analysis:**
- ⚠️ Implementation finds `GUARD_WAIT` candidates
- ⚠️ Searches depth-first, left-to-right
- ✅ Matches "leftmost" requirement
- ⚠️ No explicit "one solved clause per split" mechanism

### 5.3 Stability

**Provided Model:**
- Not explicitly defined in SYNC-based semantics
- Assumed that SPLIT-? requires `Stable(configuration)`

**Implementation** (`emulator/tree.h:66`):
```c
typedef enum {
  DEAD, STABLE, UNSTABLE, XSTABLE, XUNSTABLE
} sflag;
```

**Documentation** (`doc/internals.tex:2540-2560`):
```
Stable(G) iff:
  (i) no deterministic operation applicable to or within G
  (ii) no future environment changes can enable deterministic operations
```

**Analysis:**
- ✅ All three recognize stability concept
- ⚠️ Implementation tracks stability as flag on and-boxes
- ⚠️ Provided model assumes stability check before split
- ⚠️ Documentation has complex stability definition involving constraint theory

---

## 6. Store Management

### 6.1 Environment and Stores

**Provided Model:**
```
s: local bindings {X ↦ t, …}
S: external environment (from enclosing boxes)
Lookup: t↓(s,S) with local-first priority
```

**Implementation:**
- `envid *env`: Environment identifier (pointer to and-box)
- `unifier *unify`: Unification constraints (linked list)
- `constraint *constr`: Domain constraints (linked list)
- Variable dereferencing via tagged pointer scheme

**Documentation:**
```
Binding window: worker's view of constraints on current branch
Variables have environment identifiers
Local vs external determined by environment comparison
```

**Analysis:**
- ⚠️ Provided model: explicit store with map structure
- ⚠️ Implementation: distributed store (variables + constraint lists + trail)
- ✅ Both distinguish local vs external variables
- ⚠️ Implementation uses environment pointers, not explicit variable sets

### 6.2 Variable Ownership

**Provided Model:**
```
V: set of local variables owned by and-box
X ∈ V means X is local
Promotion: V ← V ∪ V_promoted
```

**Implementation:**
```c
#define IsLocalUVA(V,A)  (UvaEnv(V) == &((A)->env))
#define IsLocalGVA(V,A)  (GvaEnv(V) == &((A)->env))
```

**Analysis:**
- ⚠️ Provided model: explicit set membership
- ⚠️ Implementation: pointer equality check
- ✅ Both support variable ownership transfer on promotion
- ⚠️ Implementation more efficient but less explicit

---

## 7. Key Discrepancies

### 7.1 Critical Differences

| Feature | Provided Model | Implementation | Impact |
|---------|----------------|----------------|--------|
| **Global Config** | Flat multiset of and-boxes | Hierarchical tree | High - affects splitting |
| **SYNC Rule** | Explicit decomposition | Compatibility check only | High - affects guard solving |
| **?-Splitting** | Incremental (one clause) | Bulk copy | High - affects search |
| **OR-Boxes** | Eliminated by design | Used in docs, not impl | Medium |
| **Guard Operators** | !, j, ? | UNIT, ORDER, UNORDER | Medium |
| **CH-?** | Single clause, no quiet | Not present | Medium |
| **Store Representation** | Explicit map `s` | Distributed | Low - implementation detail |

### 7.2 Missing Features in Implementation

1. **Incremental ?-Splitting**: Implementation copies entire and-box, not one clause at a time
2. **Explicit SYNC**: No decomposition of external variable matches into local equations
3. **CH-? Rule**: No special handling for single-clause choice-boxes without quietness
4. **Multiset Configuration**: Uses tree instead of flat multiset
5. **Fresh Renaming Per Clause**: Happens at expansion, not per-split

### 7.3 Extra Features in Implementation

1. **GUARD_ORDER**: Ordered disjunction not in provided model or documentation
2. **Trail Mechanism**: Explicit backtracking support (necessary for Prolog compatibility)
3. **Stability Flags**: Explicit status tracking (STABLE, UNSTABLE, etc.)
4. **Task Queues**: Wake, recall, task queues for scheduling
5. **Constraint Methods**: Extensible constraint solver framework (GVA)

---

## 8. Alignment Opportunities

### 8.1 High-Priority Alignments

#### 8.1.1 Implement Explicit SYNC Rule

**Current:**
```c
// install() checks compatibility and fails
if (incompatible) goto fail;
```

**Proposed:**
```c
// Decompose external variable matches into local equations
if (X not local && X in guard_store && X in env_store) {
  Term tg = deref(guard_store[X]);
  Term te = deref(env_store[X]);
  add_equation(tg, te, guard);  // Add to guard goals
  remove_from_guard_store(X);
}
```

**Benefits:**
- Enables incremental guard solving
- Makes quietness detection more accurate
- Matches formal semantics

#### 8.1.2 Implement Incremental ?-Splitting

**Current:**
```c
// Copy entire and-box
promoted = copy(cand, exs);
```

**Proposed:**
```c
// Split only one solved clause
clause_i = find_solved_clause(choice);
A_promote = create_andbox_for_clause(clause_i);
A_rest = remove_clause(choice, i);
add_to_multiset(A_promote);
add_to_multiset(A_rest);
```

**Benefits:**
- Finer-grained search control
- Better matches formal semantics
- Potentially more efficient

#### 8.1.3 Add CH-? Promotion Rule

**Proposed:**
```c
CaseLabel(GUARD_WAIT):
  if (Solved(andb) && single_clause(chb)) {
    // Promote without quietness requirement
    promote_deterministic(andb);
  } else if (Solved(andb) && Quiet(andb) && Stable(parent)) {
    // Split for multiple clauses
    goto splithandler;
  }
```

**Benefits:**
- Matches formal semantics
- Enables deterministic promotion for single-clause wait guards

### 8.2 Medium-Priority Alignments

#### 8.2.1 Unify Guard Operator Terminology

Create explicit mapping:
- `!` (provided) ↔ `GUARD_UNIT` (impl) ↔ `cut` (docs)
- `j` (provided) ↔ `GUARD_UNORDER` (impl) ↔ `commit` (docs)
- `?` (provided) ↔ `GUARD_WAIT` (impl) ↔ `wait` (docs)

Document `GUARD_ORDER` as extension.

#### 8.2.2 Explicit Store Representation

Add explicit store structure for debugging:
```c
typedef struct store {
  struct binding *bindings;  // X ↦ t mappings
  struct store *parent;      // Parent environment
} store;
```

### 8.3 Low-Priority Alignments

#### 8.3.1 Multiset Configuration (Optional)

Consider representing global state as multiset for parallel execution.

#### 8.3.2 Documentation Updates

Update `doc/internals.tex` to reflect:
- Current guard operators
- SYNC mechanism (or lack thereof)
- Actual splitting behavior

---

## 9. Conclusion

### 9.1 Summary of Alignment

| Category | Alignment Level | Notes |
|----------|----------------|-------|
| **Core Concepts** | ✅ High (90%) | And-boxes, choice-boxes, guards all present |
| **Unification** | ✅ High (85%) | All rules implemented, SYNC partial |
| **Guard Semantics** | ⚠️ Medium (70%) | Similar but not identical operators |
| **Promotion Rules** | ⚠️ Medium (65%) | CH-! and CH-j present, CH-? missing |
| **Splitting** | ⚠️ Low (40%) | Different mechanism (bulk vs incremental) |
| **Configuration** | ⚠️ Low (30%) | Tree vs multiset |

### 9.2 Recommendations

1. **Priority 1**: Implement explicit SYNC rule for better guard solving
2. **Priority 2**: Add CH-? promotion rule for single-clause deterministic cases
3. **Priority 3**: Consider incremental splitting for ?-guards
4. **Priority 4**: Document current guard operator semantics clearly
5. **Priority 5**: Align terminology across code, docs, and formal model

### 9.3 Feasibility Assessment

- **SYNC Implementation**: Medium effort, high value
- **CH-? Rule**: Low effort, medium value
- **Incremental Splitting**: High effort, medium-high value
- **Multiset Configuration**: Very high effort, low value (fundamental redesign)

---

## References

1. **Provided Model**: User-supplied formal operational semantics
2. **Implementation**: `emulator/` directory, especially:
   - `emulator/engine.c:374-523` - Guard handlers
   - `emulator/candidate.c:51-194` - Candidate selection
   - `emulator/unify.c` - Unification engine
   - `emulator/tree.h:66-107` - And-box/Choice-box structures
3. **Documentation**: `doc/internals.tex:2240-2640` - AKL formal definition

---

**End of Comparison**
