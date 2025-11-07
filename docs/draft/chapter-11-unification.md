# Chapter 11: Unification

## Introduction

**Unification** is the fundamental operation in logic programming. It's how we match patterns, bind variables, and propagate constraints. Every predicate call, every clause selection, every data structure construction involves unification. Understanding AGENTS' unification engine is essential to understanding how the system works.

This chapter dissects the unification algorithm, explains how it handles the myriad cases (variable-variable, variable-term, structure-structure), deals with cycles and sharing, and integrates with constraints and the trail.

## What is Unification?

### Formal Definition

Two terms `X` and `Y` **unify** if there exists a **substitution** θ such that Xθ = Yθ.

**Example 1**: Simple unification
```
X = 5
X is unbound → bind X to 5
Result: X = 5, substitution {X ← 5}
```

**Example 2**: Structure unification
```
f(X, 2) = f(1, Y)
Match functor: f = f ✓
Match arity: 2 = 2 ✓
Unify arguments:
  X = 1 → bind X to 1
  2 = Y → bind Y to 2
Result: X = 1, Y = 2, substitution {X ← 1, Y ← 2}
```

**Example 3**: Failure
```
f(X) = g(Y)
Match functor: f ≠ g ✗
Result: failure (no substitution exists)
```

### Occurs Check

The **occurs check** prevents infinite structures:

```
X = f(X)
```

**With occurs check**: X occurs in f(X), creating a cycle → fail

**Without occurs check**: Create cyclic structure (rational tree)

AGENTS supports **rational trees** (cyclic structures), so the occurs check is **optional** and typically disabled for performance.

## The Unification Algorithm

### Entry Point

From `unify.c:55-64`:

```c
bool unify(x, y, andb, exs)
     andbox *andb;
     Term x,y;
     exstate *exs;
{
    register bool res;
    CheapUnify_ELSE(x, y, andb, res)
        UNIFY_INTERNAL(res)
    return res;
}
```

**Two paths:**

1. **CheapUnify**: Fast path for common simple cases
2. **UNIFY_INTERNAL**: Full algorithm for complex cases

### Cheap Unification

The **cheap path** handles common cases without heavy machinery:

From `unify.h` (conceptual):

```c
#define CheapUnify_ELSE(x, y, andb, res) \
    if (Eq(x, y)) { \
        res = TRUE; \
    } else if (IsREF(x) && VarIsUVA(Ref(x))) { \
        /* Bind local unconstrained variable */ \
        BindCheck(Ref(x), y, res = FALSE; goto cheap_done); \
        res = TRUE; \
    } else if (IsREF(y) && VarIsUVA(Ref(y))) { \
        /* Bind local unconstrained variable */ \
        BindCheck(Ref(y), x, res = FALSE; goto cheap_done); \
        res = TRUE; \
    } else
cheap_done:
```

**Cases handled:**
1. **Identical terms**: `X` = `X` → success (pointer equality)
2. **Unbound var = term**: Bind variable, trail if needed
3. **Term = unbound var**: Symmetric case

**Why "cheap"?** No stack allocation, no hash table, just simple pointer operations.

**Coverage**: ~70-80% of unifications in typical programs.

### Full Unification Algorithm

For complex cases (structures, lists, constrained variables, generic objects):

From `unify.c:82-250` (simplified structure):

```c
bool unify_more(x, y, andb, exs)
     andbox *andb;
     Term x,y;
     exstate *exs;
{
    struct unifystate *u = &exs->urec;

loop:
    // 1. Check identity
    if (Eq(x, y))
        goto succeed;

    // 2. Variable-variable cases (many subcases)
    if (IsREF(y)) {
        if (IsREF(x)) {
            /* UVA-UVA, UVA-SVA, SVA-SVA, CVA-CVA, etc. */
            /* ~60 lines of case analysis */
            goto succeed;
        } else {
            /* Variable-term */
            goto BindTerm;
        }
    } else if (IsREF(x)) {
        /* Term-variable */
        goto BindTerm;
    }

    // 3. Non-variable unification
    SwitchTag(x, ..., immcase, bigcase, fltcase, lstcase, strcase, gencase);

immcase:  // Atoms, small integers
    goto failure;  // Eq test already done

bigcase:  // Bignums
    if (IsBIG(y) && bignum_compare(x, y) == 0)
        goto succeed;
    goto failure;

fltcase:  // Floats
    if (IsFLT(y) && FltVal(Flt(x)) == FltVal(Flt(y)))
        goto succeed;
    goto failure;

lstcase:  // Lists [H|T]
    if (IsLST(y)) {
        /* Push T arguments to stack */
        /* Unify H arguments recursively */
        goto loop;
    }
    goto failure;

strcase:  // Structures f(...)
    if (IsSTR(y)) {
        if (StrFunctor(x) == StrFunctor(y)) {
            /* Push all but first arg to stack */
            /* Unify first arg recursively */
            goto loop;
        }
    }
    goto failure;

gencase:  // Generic objects
    if (Gen(x)->method->unify(x, y, andb, exs))
        goto succeed;
    else
        goto failure;

succeed:
    return TRUE;
failure:
    return FALSE;
}
```

**Key structure**: Iterative loop with explicit stack for structure arguments.

## Variable-Variable Unification

### The Complexity

Unifying two variables has **many cases** depending on their types:

**Variable types:**
- **UVA**: Unconstrained unbound variable
- **SVA**: Constrained variable (finite domain, port, etc.)
- **CVA**: Custom constrained variable (user-defined method)

**Locality:**
- **Local**: Created in current and-box
- **External**: Created in parent or sibling and-box

**Total combinations**: 3 types × 2 localities × 2 variables = **12 basic cases**, with many subcases.

### Binding Rules

From `unify.c:100-160` comments:

```c
/* The rules are:
 *
 * Bind a local UVA or SVA to any external variable.
 * Bind an UVA to a SVA.
 * Bind an UVA or SVA to any CVA variable.
 * Unify two CVA variables with a unify method call.
 */
```

**Why these rules?**

1. **Local to external**: Prefer binding local variables (easier to undo on backtracking)
2. **UVA to SVA**: Unconstrained → constrained (add constraints, don't lose them)
3. **UVA/SVA to CVA**: Standard variables → custom (custom method handles)
4. **CVA to CVA**: Call custom unification method (application-specific logic)

### UVA-UVA Case

From `unify.c:108-118`:

```c
if (VarIsUVA(xref)) {
    DerefUvaEnv(xref);
    if (IsLocalUVA(xref, andb)) {
        // x is local UVA → bind to y
        BindCheck(xref, y, goto failure);
    } else if (VarIsUVA(yref)) {
        // Both external UVA → bind y to x
        BindCheckUVA(yref, x, exs, andb, goto failure);
    } else if (GvaIsSva(RefGva(yref))) {
        // x external UVA, y is SVA → bind y to x
        BindCheckSVA(yref, x, exs, andb, goto failure);
    } else {
        // x external UVA, y is CVA → bind x to y
        BindCheckUVA(xref, y, exs, andb, goto failure);
    }
}
```

**Decision tree**: Determine which variable to bind based on locality and constraint status.

### SVA-SVA Case

From `unify.c:121-145`:

```c
if (GvaIsSva(RefGva(xref))) {
    if (IsLocalGVA(xref, andb)) {
        // x is local SVA
        if (VarIsUVA(yref)) {
            // y is UVA → bind y to x
            /* ... */
        } else {
            // y is also SVA/CVA → bind x to y
            BindCheckSVA(xref, y, exs, andb, goto failure);
        }
    } else {
        // x is external SVA
        /* ... handle external binding ... */
    }
}
```

**Constraint variables**: When binding, must wake any suspended agents waiting on that variable.

### CVA-CVA Case

From `unify.c:150-158`:

```c
else if (IsLocalGVA(xref, andb)) {
    // We prefer to use the local variable's method
    if (!(RefCvaMethod(xref)->unify(x, y, andb, exs))) {
        goto failure;
    }
} else {
    if (!(RefCvaMethod(yref)->unify(y, x, andb, exs))) {
        goto failure;
    }
}
```

**Custom unification**: Delegate to the variable's custom method (for user-defined constraint domains).

## Structure Unification

### List Unification

From `unify.c:191-232`:

```c
lstcase:
    if (IsLST(y)) {
        xtuple = LstCdrRef(Lst(x));   // Pointer to tail of x
        ytuple = LstCdrRef(Lst(y));   // Pointer to tail of y
        index = 1;                     // 1 argument left (tail)

    do_tuples:
        // 1. Hash this pair to detect cycles
        hval = TermHashValue(x);
        TermHashLookup(u->table, x, hval, eptr, goto not_found);
        // Found in hash table → already unifying this pair
        x = GetEntryTerm(eptr);   // Get corresponding y term
        goto loop;

    not_found:
        // 2. Record this pair
        TermHashEnter(u->table, eptr, x, y);
        u->table_used_flag = 1;

        // 3. Push tail to stack (if more than one argument)
        if (index > 0) {
            u->sp += 1;
            u->sp->tuple1 = xtuple;
            u->sp->tuple2 = ytuple;
            u->sp->index = index;
        }

        // 4. Unify head (first argument)
        GetArg(x, xtuple - index);
        Deref(x, x);
        GetArg(y, ytuple - index);
        Deref(y, y);
        goto loop;   // Tail recursion on head
    }
    goto failure;
```

**Algorithm:**

1. **Check if both are lists** (if not, fail)
2. **Hash pair** to detect if we've seen this combination before
3. **If seen**: Follow the mapping (handles cycles)
4. **If new**: Record pair, push tail to stack, unify head recursively

**Why hash table?** To handle **cyclic structures** and **shared substructures**.

### Structure Unification

From `unify.c:234-244`:

```c
strcase:
    if (IsSTR(y)) {
        Structure xstr = Str(x), ystr = Str(y);
        if (StrFunctor(xstr) == StrFunctor(ystr)) {
            index = StrArity(xstr) - 1;
            xtuple = StrArgRef(xstr, index);
            ytuple = StrArgRef(ystr, index);
            goto do_tuples;
        }
    }
    goto failure;
```

**Process:**

1. **Check both are structures** (fail if not)
2. **Check same functor** (fail if different)
3. **Unify arguments** using same tuple logic as lists

**Optimization**: Reuse `do_tuples` code for both lists and structures.

## Cycle Detection

### The Hash Table

From `exstate.h` (unifystate structure):

```c
struct unifystate {
    unify_entry  *stack;           // Stack for arguments
    unify_entry  *sp;              // Stack pointer
    unify_entry  *stack_end;       // Stack limit
    long         stack_length;     // Stack size

    hash_table   *table;           // Hash table for cycles
    int          table_used_flag;  // Did we use the table?
};
```

**Hash table**: Maps `(term_x, term_y)` pairs to record which structures we're currently unifying.

**Why needed?** Consider:

```
X = f(X)
Y = f(Y)
Unify X with Y
```

**Without cycle detection:**
```
Unify X with Y
  → Unify f(X) with f(Y)
    → Unify X with Y      ← infinite recursion!
      → Unify f(X) with f(Y)
        → ...
```

**With cycle detection:**
```
Unify X with Y
  Hash: record (X, Y)
  → Unify f(X) with f(Y)
      Get arg: X', Y'
      → Unify X' with Y'
          Hash lookup: (X', Y') → already recorded!
          → Return success (cycle detected)
```

**Result**: Cyclic structures unify correctly.

### Clearing the Hash Table

After unification completes:

From `unify.c:38-41`:

```c
if (exs->urec.table_used_flag) {
    clear_hash_table(exs->urec.table);
    exs->urec.table_used_flag = 0;
}
```

**Why clear?** Hash table is reused across unifications. Must clear old entries before next unification.

**Optimization**: Only clear if actually used (most unifications don't need it).

## The Trail

### What is the Trail?

The **trail** records variable bindings so they can be **undone on backtracking**.

From `exstate.h`:

```c
typedef struct trailentry {
    Reference  var;      // Pointer to variable cell
    Term       oldval;   // Original value (before binding)
} trailentry;

typedef struct {
    trailentry  *start;
    trailentry  *top;
    trailentry  *end;
} trailhead;
```

**Trail operations:**

```c
// Record binding
#define TrailRef(exs, ref) \
{ \
    (exs)->trail.top++; \
    (exs)->trail.top->var = (ref); \
    (exs)->trail.top->oldval = *(ref); \
}

// Undo bindings
void unwind_trail(trailhead *trail, trailentry *mark)
{
    while (trail->top != mark) {
        trail->top->var->value = trail->top->oldval;
        trail->top--;
    }
}
```

### When to Trail?

**Rule**: Trail a binding if it might need to be undone on backtracking.

**Cases:**

1. **Local variable**: Don't trail (will be discarded with and-box)
2. **External variable**: **Trail** (might backtrack to before this and-box)
3. **Constrained variable**: **Trail and wake** (suspended agents must be woken)

From `unify.c:170-172`:

```c
BindTerm:
    /* Bind xref to y */
    BindVariable(exs, andb, xref, y, goto failure);
    goto succeed;
```

**BindVariable macro**:

```c
#define BindVariable(exs, andb, ref, term, FailLabel) \
{ \
    if (IsLocalVar(ref, andb)) { \
        /* Local - just bind */ \
        *(ref) = (term); \
    } else { \
        /* External - trail then bind */ \
        TrailRef(exs, ref); \
        *(ref) = (term); \
    } \
}
```

### Trail and Backtracking

When backtracking to a choice point:

```c
void backtrack_to_choice(choicebox *chb)
{
    // Restore trail to saved point
    unwind_trail(&trail, chb->trail.top);

    // Restore argument registers
    for (int i = 0; i < arity; i++) {
        areg[i] = chb->areg[i];
    }

    // Try next clause
    pc = chb->next;
}
```

**Effect**: All bindings made since the choice point are undone, restoring variables to unbound state.

## Constraint Integration

### Constrained Variables (SVA)

When binding a constrained variable, must **wake suspended agents**:

From `unify.c:74-80`:

```c
#define BindCheckSVA(V, T, W, A, BindFail) \
    if (IsLocalGVA(V, A)) { \
        WakeAll(W, V);              /* Wake suspended agents */ \
        BindCheck(V, T, BindFail); \
    } else if (!bind_external_sva(W, A, V, T)) { \
        BindFail; \
    }
```

**WakeAll**: Traverse the variable's suspension list, reactivate all waiting agents.

**Example:**

```
fd(X in 1..10, Y in 1..10, X =:= Y + 1)
```

When `X` is bound to `5`:
1. Bind `X = 5`
2. Wake constraints waiting on `X`
3. Constraint `X =:= Y + 1` wakes
4. Propagates: `5 =:= Y + 1` → `Y in {4}`

### Custom Variable Methods (CVA)

From `unify.c:152-158`:

```c
if (!(RefCvaMethod(xref)->unify(x, y, andb, exs))) {
    goto failure;
}
```

**CVA method table**:

```c
typedef struct {
    bool (*unify)(Term, Term, andbox *, exstate *);
    void (*gc_external)(Generic, gcstate *);
    /* ... more methods ... */
} method;
```

**Use case**: User-defined constraint domains (e.g., floating-point intervals, symbolic algebra).

## Performance Optimizations

### 1. Cheap Unification Fast Path

From `unify.c:61`:

```c
CheapUnify_ELSE(x, y, andb, res)
    UNIFY_INTERNAL(res)
```

**Impact**: ~70-80% of unifications handled without allocating stack or hash table. **10-20× faster** for simple cases.

### 2. Tail Recursion via goto

From `unify.c:231`:

```c
GetArg(x, xtuple - index);
Deref(x, x);
GetArg(y, ytuple - index);
Deref(y, y);
goto loop;   // Tail recursion
```

**Why goto?** Avoids C function call overhead. Effectively turns recursion into iteration.

**Benefit**: ~2-5× faster than recursive calls.

### 3. Hash Table Lazy Initialization

From `unify.c:204`:

```c
TermHashEnter(u->table, eptr, x, y);
u->table_used_flag = 1;
```

**Observation**: Most unifications don't involve cycles or sharing.

**Optimization**: Only use hash table when actually needed. Set flag when used, clear later.

**Benefit**: Avoid hash table overhead (~50% of unifications).

### 4. Explicit Stack for Arguments

From `unify.c:209-226`:

```c
u->sp += 1;
u->sp->tuple1 = xtuple;
u->sp->tuple2 = ytuple;
u->sp->index = index;
if (u->sp == u->stack_end) {
    /* Grow the stack... */
}
```

**Why explicit stack?** Avoids C call stack limits. Can unify arbitrarily deep structures.

**Trade-off**: Slightly slower than recursion, but more robust.

## Edge Cases and Subtleties

### Dereferencing

Before comparing or binding:

```c
Deref(x, x);
Deref(y, y);
```

**Why?** Variables may be bound to references which are bound to other references, etc. Must follow the chain to the end.

**Macro** (from `term.h`):

```c
#define Deref(t, result) \
do { \
    while (IsREF(t)) { \
        Term *addr = RefOf(t); \
        if (*addr == t) break;  /* Unbound */ \
        t = *addr; \
    } \
    result = t; \
} while (0)
```

### Shared Substructures

```
  f
 / \
A   B
 \ /
  C   ← Shared
```

**Hash table handles this**:

1. Unify `A` with `X`: Record (`A`, `X`)
2. Unify `B` with `Y`: Record (`B`, `Y`)
3. Unify `C` (from `A`) with `C'` (from `X`): Record (`C`, `C'`)
4. Unify `C` (from `B`) with `C'` (from `Y`): **Found in hash** → success

**Result**: Sharing preserved.

### Environment Variables

Some variables belong to specific environments (and-boxes). Must check locality:

```c
#define IsLocalUVA(ref, andb) \
    (RefEnv(ref) == andb)
```

**Why?** Determines whether to trail the binding.

## Unification in Context

### In Clause Matching

```
% Clause head: foo(X, f(Y))
foo(A, B) := ...

% Match call: foo(1, f(2))
```

**Process:**
1. Unify `A` with `1` → bind `A = 1`
2. Unify `B` with `f(2)`
   → Match structure `f(_)` with `f(2)`
   → Unify `Y` with `2` → bind `Y = 2`

### In Constraint Propagation

```
fd(X in 1..10, Y in 1..10, X =:= Y)
% Later: Y = 5
```

**Process:**
1. Unify `Y` with `5` → bind `Y = 5`
2. Wake constraints on `Y`
3. Constraint `X =:= Y` wakes
4. Propagate: `X =:= 5` → bind `X = 5`

### In Built-in Predicates

```
?- X = [1,2,3], append(X, [4,5], Y).
```

**Process:**
1. Unify `X` with `[1,2,3]` → bind `X`
2. Call `append/3`
3. Inside `append`, unify arguments with clause heads
4. Construct result, unify with `Y`

## Summary

AGENTS' unification engine is **sophisticated and efficient**:

- **Two-tier approach**: Cheap fast path + full algorithm
- **Handles all cases**: Variable-variable (12+ subcases), structure-structure, lists
- **Cycle detection**: Hash table records seen pairs
- **Trailing**: Records bindings for backtracking
- **Constraint integration**: Wakes suspended agents, calls custom methods
- **Optimizations**: Tail recursion via goto, lazy hash table, explicit stack
- **Edge cases**: Shared substructures, environment locality, dereferencing

Understanding unification is key to understanding logic programming. It's the operation that makes pattern matching, data construction, and constraint solving possible.

The next chapter explores how backtracking and choice points build on unification to provide nondeterministic search.

---

**Next Chapter**: [Chapter 12: Choice Points and Backtracking](chapter-12-backtracking.md)
