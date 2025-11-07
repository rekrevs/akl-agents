# Chapter 12: Choice Points and Backtracking

## Introduction

AGENTS implements **nondeterministic search** through a sophisticated choice point and backtracking mechanism. When a goal has multiple matching clauses, the system creates a **choice-box** that represents the set of alternatives, and systematically tries each one until success or exhaustion.

This mechanism is fundamental to logic programming: it allows programs to express **search** declaratively. The program says *what* solutions look like, and the system automatically explores the search space to find them.

But AGENTS goes beyond traditional Prolog in several ways:

1. **Concurrent alternatives**: Multiple clauses can exist as **sibling and-boxes** simultaneously
2. **Non-chronological backtracking**: The agent tree structure allows more flexible control
3. **Guard-based commitment**: Clauses can **commit** based on guards, pruning alternatives
4. **Trail mechanism**: Variable bindings are recorded for efficient undo

This chapter explores the **choice point mechanism** in detail: how choices are created (TRY/RETRY/TRUST instructions), how state is saved, and how backtracking restores previous states.

## Choice Point Architecture

### The Problem

Consider the Prolog program:

```prolog
member(X, [X|_]).
member(X, [_|Xs]) :- member(X, Xs).
```

When `member(1, [2,1,3])` is called, two clauses match. The system must:

1. **Try the first clause**: Attempt to unify `1` with `2` (fails)
2. **Save state**: Remember we need to try the second clause
3. **Backtrack**: Undo any bindings from the failed attempt
4. **Try next clause**: Attempt `member(1, [1,3])`
5. **Continue**: Recursively search

This requires:
- **Saving arguments**: The original `X` and `Xs` values
- **Saving program counter**: Where to resume for next clause
- **Saving variable bindings**: What to undo on backtracking
- **Managing alternatives**: Which clauses remain to try

### Choice-Box Structure

AGENTS uses **choice-boxes** to manage alternatives (from `emulator/tree.h:95-107`):

```c
typedef struct choicebox {
  struct choicecont *cont;        // Saved state (continuation)
  struct choicebox  *next;        // Next sibling choice-box
  struct choicebox  *previous;    // Previous sibling
  struct andbox     *father;      // Parent and-box
  struct predicate  *def;         // Predicate definition
  struct andbox     *tried;       // List of alternative and-boxes
  int               type;         // Choice-box type
} choicebox;
```

**Key fields:**

- **cont**: The **choice continuation** holding saved arguments and program counter
- **tried**: The list of alternative **and-boxes** (one per clause)
- **father**: The parent and-box that created this choice
- **def**: The predicate being called

### Choice Continuation

The **choice continuation** saves the state needed for backtracking (from `emulator/tree.h:111-118`):

```c
typedef struct choicecont {
  union {
    code  *label;      // Program counter for next clause
    bool  (*cinfo)();  // Or: built-in continuation function
  } instr;
  int   arity;         // Number of arguments
  Term  arg[ANY];      // Saved argument registers (X0..XN)
} choicecont;
```

**Purpose:**

- **label**: Points to the code for the next alternative clause
- **arg[]**: Holds the **original argument values** (before any unification)
- **arity**: How many arguments to save/restore

This structure is **variable-sized**:

```c
#define ChoiceContSize(Arity) \
    (sizeof(choicecont) + sizeof(Term) * ((Arity) - ANY))
```

For a 3-argument predicate, the choice continuation holds 3 Term values.

## The TRY/RETRY/TRUST Instructions

AGENTS uses three instructions to manage choice points, following the WAM tradition:

1. **TRY**: First clause, create choice-box
2. **RETRY**: Middle clause, add alternative
3. **TRUST**: Last clause, finalize

### Bytecode Example

The `member/2` predicate compiles to:

```
member/2:
    TRY L1          % Try first clause, save state for L2
L1:
    % First clause: member(X, [X|_])
    GET_LIST X1
    UNIFY_VARIABLE X0
    UNIFY_VOID 1
    PROCEED

L2:
    RETRY L3        % Try second clause, save state for L3 (none)
L3:
    % Second clause: member(X, [_|Xs])
    GET_LIST X1
    UNIFY_VOID 1
    UNIFY_VARIABLE X1
    CALL member/2
    PROCEED
```

Wait, actually `member/2` has only 2 clauses, so it would be:

```
member/2:
    TRY L1          % Try first clause, save state for next
L1:
    % First clause body
    ...
    PROCEED

L2:
    TRUST L3        % Last clause, no more alternatives
L3:
    % Second clause body
    ...
    PROCEED
```

### TRY Instruction

**Semantics**: Create a choice-box for the first clause, saving state for backtracking.

**Implementation** (from `emulator/engine.c:286-316`):

```c
CaseLabel(TRY):
  /* Build a choice-box and set up execution for the first clause. */
  {
    code *label;

    // Fetch label for next alternative
    FetchTryBranchLabel(label);

    // Save current task/wake/recall/trail pointers
    PushContext(exs);

    // Create choice-box structure
    MakeChoicebox(chb, def, andb);

    // Move pc past TRY instruction
    pc += TryInstructionSize;

    // Create choice continuation with saved args
    MakeChoiceCont(chb, pc, areg, arity);

    // Insert choice-box into tree
    InsertChoicebox(chb, insert, andb);

    // Set pc to first clause body
    pc = label;
    FetchOpCode(0);   // Prefetch next opcode

    // Create and-box for first clause
    MakeAndbox(andb, chb);

    insert = NULL;
    chb->tried = andb;

    NextOp();    // Execute first clause
  }
```

**Step by step:**

1. **Fetch label**: Get code pointer for next alternative (from bytecode)
2. **Push context**: Save current stack pointers (trail, task queue, etc.)
3. **Create choice-box**: Allocate structure, set father to current and-box
4. **Save arguments**: Copy `areg[0..arity-1]` into choice continuation
5. **Save program counter**: Store where to resume for next clause
6. **Create and-box**: Allocate structure for first clause body
7. **Execute**: Jump to first clause code

**Key macros:**

```c
#define MakeChoicebox(Chb, Def, Andb) \
{ \
  NewChoicebox(Chb); \
  Chb->def = Def; \
  Chb->father = Andb; \
  Chb->tried = NULL; \
  Chb->cont = NULL; \
  Chb->type = 0; \
}
```

```c
#define MakeChoiceCont(Chb, Label, Areg, Arity) \
{ \
  int i; \
  choicecont *cont; \
  NewChoiceCont(cont, Arity);           // Allocate variable-sized \
  (Chb)->cont = cont; \
  cont->instr.label = Label;            // Save pc \
  cont->arity = Arity; \
  for(i = 0; i != Arity; i++) \
    cont->arg[i] = (Areg)[i];           // Save arguments \
}
```

### RETRY Instruction

**Semantics**: Try the next clause, creating a sibling and-box.

**Implementation** (from `emulator/engine.c:318-346`):

```c
CaseLabel(RETRY):
  /* Try the next clause */
  {
    andbox *prev;
    code *label;

    prev = andb;    // Current and-box (failed alternative)

    // Fetch label for next clause
    FetchTryBranchLabel(label);

    // Restore original arguments from choice continuation
    RestoreAreg(chb, areg);

    // Move pc past RETRY instruction
    pc += TryInstructionSize;

    // Update choice continuation to point to next alternative
    SetChoiceContLabel(chb, pc);

    // Set pc to current clause body
    pc = label;
    FetchOpCode(0);   // Prefetch opcode

    // Create and-box for this clause
    MakeAndbox(andb, chb);

    // Insert as sibling of previous and-box
    InsertAndbox(andb, prev, chb);

    insert = NULL;

    NextOp();    // Execute current clause
  }
```

**Key differences from TRY:**

1. **No PushContext**: We're at the same choice level
2. **RestoreAreg**: Restore original argument values (undoing any bindings from previous clause)
3. **InsertAndbox**: Add as **sibling** to previous and-box
4. **Update continuation**: Point to next alternative

**RestoreAreg macro** (from `emulator/config.h:484-489`):

```c
#define RestoreAreg(Chb, Areg) \
{ \
  register int i; \
  for(i = 0; i != (Chb)->cont->arity; i++) \
    (Areg)[i] = (Chb)->cont->arg[i];    // Copy from saved state \
}
```

This **restores** the original arguments, as if the previous clause never executed.

### TRUST Instruction

**Semantics**: Last clause, no more alternatives—remove choice continuation.

**Implementation** (from `emulator/engine.c:347-370`):

```c
CaseLabel(TRUST):
  /* This is the last clause */
  {
    andbox *prev;

    prev = andb;

    // Fetch label for last clause
    FetchTryBranchLabel(pc);
    FetchOpCode(0);   // Prefetch opcode

    // Restore original arguments
    RestoreAreg(chb, areg);

    // Remove choice continuation (no more alternatives)
    RemoveChoiceCont(chb);

    // Create and-box for last clause
    MakeAndbox(andb, chb);

    // Insert as sibling
    InsertAndbox(andb, prev, chb);

    insert = NULL;

    NextOp();    // Execute last clause
  }
```

**Key difference:**

- **RemoveChoiceCont**: Marks that no alternatives remain

**RemoveChoiceCont** (from `emulator/config.h:498-521`):

```c
#ifdef TRACE
#define RemoveChoiceCont(Chb) \
{ \
  (Chb)->cont->instr.label = NULL;    // NULL = no more alternatives \
}
#else
#define RemoveChoiceCont(Chb) \
{ \
  (Chb)->cont = NULL;                 // Free continuation structure \
}
#endif
```

In trace mode, the continuation is kept (for debugging) but marked NULL. Otherwise, it's freed.

### TRY_SINGLE Instruction

For predicates with **only one clause**, AGENTS optimizes with TRY_SINGLE (from `emulator/engine.c:260-284`):

```c
CaseLabel(TRY_SINGLE):
  /* Build a choice-box and set up execution for the only clause. */

  FetchTryBranchLabel(pc);
  FetchOpCode(0);

  PushContext(exs);

  MakeChoicebox(chb, def, andb);

  // Create EMPTY continuation (no alternatives)
  MakeEmptyChoiceCont(chb, areg, arity);

  InsertChoicebox(chb, insert, andb);

  MakeAndbox(andb, chb);

  insert = NULL;
  chb->tried = andb;

  NextOp();
```

This creates a choice-box but with an **empty continuation** (no saved state for backtracking).

## The Trail Mechanism

When a variable is bound during execution, the binding must be **recorded** so it can be **undone** if backtracking occurs.

### Trail Structure

**Definition** (from `emulator/exstate.h:286-353`):

```c
typedef struct trailentry {
  Term  var;    // Variable that was bound
  Term  val;    // Value it was bound to (for restore)
} trailentry;

typedef struct trailhead {
  int          size;
  trailentry   *start;
  trailentry   *current;    // Stack pointer
  trailentry   *end;
} trailhead;
```

The trail is a **stack** of variable bindings.

### Trail Operations

**Recording a binding** (from `emulator/exstate.h:16-26`):

```c
#define TrailValue(W, V, X) \
{ \
  (W)->trail.current++; \
  if((W)->trail.current == (W)->trail.end) { \
    reinit_trail(W); \
  } \
  (W)->trail.current->var = (V); \
  (W)->trail.current->val = (X); \
}

#define TrailRef(W, V)  TrailValue(W, TagRef(V), RefTerm(V))
```

When unification binds a variable, it calls:

```c
TrailRef(exs, variable_address);
```

This pushes the variable's address and current value onto the trail.

**Checking if trail is empty** (from `emulator/exstate.h:13`):

```c
#define EmptyTrail(W) \
    ((W)->context.current->trail == (W)->trail.current)
```

If the trail pointer equals the context's saved trail pointer, no bindings have been made since that choice point.

### Trail Unwinding

**Undoing bindings** (from `emulator/exstate.h:28-37`):

```c
#define UndoTrail(W) \
{ \
  register trailentry *tr, *stop; \
  stop = (W)->context.current->trail;    // Trail pointer at choice point \
  tr = (W)->trail.current;               // Current trail top \
  for(; tr != stop; tr--) { \
    Bind(Ref(tr->var), tr->val);         // Restore binding \
  } \
  (W)->trail.current = stop;             // Reset trail pointer \
}
```

**Algorithm:**

1. Get saved trail pointer from context stack
2. Iterate backwards from current trail top to saved pointer
3. For each entry, **restore** the variable's binding to its old value
4. Reset trail pointer to saved position

This **undoes** all variable bindings made since the choice point was created.

### Example

```prolog
p(X) :- X = 1.
p(X) :- X = 2.
```

When `p(Y)` is called:

1. **TRY**: Create choice-box, save `Y` (unbound)
2. **Execute first clause**: Bind `Y = 1`, **trail** the binding
3. **First clause succeeds**
4. *(User asks for more solutions)*
5. **Backtrack**: **UndoTrail** restores `Y` to unbound
6. **RETRY**: Restore arguments (already done by UndoTrail)
7. **Execute second clause**: Bind `Y = 2`

The trail ensures `Y` is unbound when trying the second clause.

## The Context Stack

The **context stack** saves the state of various execution stacks at each choice point.

### Context Structure

**Definition** (from `emulator/exstate.h:317-322`):

```c
typedef struct context {
  task        *task;      // Task queue pointer
  recall      *recall;    // Recall stack pointer
  wake        *wake;      // Wake queue pointer
  trailentry  *trail;     // Trail stack pointer
} context;

typedef struct contexthead {
  int      size;
  context  *start;
  context  *current;      // Stack pointer
  context  *end;
} contexthead;
```

**Purpose**: When a choice point is created, the system **saves** the current pointers for all major stacks. On backtracking, these are **restored**.

### Pushing Context

**Macro** (from `emulator/exstate.h:249-258`):

```c
#define PushContext(W) \
{ \
  (W)->context.current++; \
  if((W)->context.current == (W)->context.end) { \
    reinit_context(W); \
  } \
  (W)->context.current->task = (W)->task.current; \
  (W)->context.current->recall = (W)->recall.current; \
  (W)->context.current->wake = (W)->wake.current; \
  (W)->context.current->trail = (W)->trail.current; \
}
```

Called by **TRY** instruction to save current stack state.

### Popping Context

**Macro** (from `emulator/exstate.h:281-282`):

```c
#define PopContext(W) \
  (W)->context.current--;
```

When a choice-box is removed (all alternatives exhausted or committed), the context is popped.

### Why Context Stack?

Consider nested choices:

```prolog
p :- q, r.
q :- a.
q :- b.
r :- c.
r :- d.
```

This creates a **tree** of choice-boxes:

```
Choice(p)
    |
And-box(q,r)
    |
Choice(q)
  |     |
  a     b
    |
Choice(r)
  |     |
  c     d
```

Each choice point needs its **own** trail context. The context stack maintains this hierarchy:

- Context 0: Before any choices
- Context 1: After creating choice for `q`
- Context 2: After creating choice for `r`

When backtracking from `r`, we restore Context 1. When backtracking from `q`, we restore Context 0.

## Backtracking Algorithm

When a goal **fails**, AGENTS backtracks to find the next alternative.

### Triggering Backtracking

Failure occurs when:

1. **Unification fails**: Terms don't match
2. **Built-in fails**: Explicit failure (`fail/0`, arithmetic constraint unsatisfied)
3. **No clauses match**: Predicate call with no matching clause
4. **Guard fails**: Guard condition not satisfied

### Backtracking Process

**From choicehandler** (from `emulator/engine.c:3458-3565`):

```c
// After processing task queue, check if alternatives remain
if(NonEmptyChoiceCont(chb)) {
  // Choice-box has more alternatives to try

  code *label;

  // Undo all bindings since this choice point
  UndoTrail(exs);

  // Get label for next clause
  label = GetChoiceContLabel(chb);

  // Restore original arguments
  RestoreAreg(chb, areg);

  // Create new and-box for next alternative
  MakeAndbox(andb, chb);

  // Set program counter to next clause
  pc = label;
  FetchOpCode(0);
  yreg = andb->cont->yreg;

  insert = NULL;
  NextOp();    // Execute next clause
}
```

**Steps:**

1. **Check for alternatives**: Does choice-box have a continuation?
2. **Undo trail**: Restore all variable bindings
3. **Get next clause**: Fetch program counter from continuation
4. **Restore arguments**: Reset argument registers to original values
5. **Create and-box**: New agent for this alternative
6. **Execute**: Jump to next clause code

### No More Alternatives

If `NonEmptyChoiceCont(chb)` is false, no alternatives remain. The system:

1. **Removes the choice-box**
2. **Pops the context stack**
3. **Continues with parent choice-box** (if any)
4. **Or fails completely** (if no parent)

### Example Trace

```prolog
member(X, [X|_]).
member(X, [_|Xs]) :- member(X, Xs).

?- member(1, [2,3,1]).
```

**Execution trace:**

1. **Call** `member(1, [2,3,1])`
2. **TRY**: Create choice-box, save arguments
3. **First clause**: Try `1 = 2` → **fails**
4. **Backtrack**: UndoTrail (nothing to undo), RestoreAreg
5. **TRUST**: Last clause, remove continuation
6. **Execute** `member(1, [3,1])`
7. **Recursive call** → TRY again
8. **First clause**: Try `1 = 3` → **fails**
9. **Backtrack**: UndoTrail, RestoreAreg
10. **TRUST**: Execute `member(1, [1])`
11. **Recursive call** → TRY again
12. **First clause**: Try `1 = 1` → **succeeds!**
13. **Return** success

The trail ensures variables are restored at each backtracking step.

## Advanced Features

### Conditional Backtracking

AGENTS supports **pruning** of alternatives through:

1. **Cut** (`!`): Removes choice-boxes
2. **Guards**: Commit to one clause based on conditions
3. **Constraints**: Fail early when constraints unsatisfiable

### Trail Suspension

During guard evaluation, bindings may be **suspended** rather than committed (from `emulator/exstate.h:49`):

```c
#define SuspendTrail(W, A) suspend_trail(W, A);
```

This allows guards to **test** bindings without committing to them.

### Non-Chronological Backtracking

Because choice-boxes form a **tree** rather than a stack, AGENTS can backtrack non-chronologically. Intelligent backtracking strategies can:

- **Jump** to specific choice points
- **Prune** entire subtrees
- **Reorder** alternatives based on heuristics

This is more flexible than Prolog's chronological backtracking.

## Implementation Details

### Choice-Box Insertion

**Macro** (from `emulator/config.h`):

```c
#define InsertChoicebox(C, Ins, Andb) \
{ \
  if(Ins == NULL) { \
    (C)->next = (Andb)->tried; \
    (C)->previous = NULL; \
    if((Andb)->tried != NULL) \
      (Andb)->tried->previous = (C); \
    (Andb)->tried = (C); \
  } else { \
    (C)->next = (Ins)->next; \
    (C)->previous = (Ins); \
    if((Ins)->next != NULL) \
      (Ins)->next->previous = (C); \
    (Ins)->next = (C); \
  } \
}
```

Choice-boxes are inserted into a **doubly-linked list** at a specified position (or at the head if `Ins == NULL`).

### And-Box Insertion

**Macro**:

```c
#define InsertAndbox(A, Prev, Chb) \
{ \
  (A)->next = (Prev)->next; \
  (A)->previous = (Prev); \
  if((Prev)->next != NULL) \
    (Prev)->next->previous = (A); \
  (Prev)->next = (A); \
}
```

And-boxes are inserted as **siblings** in the tried list of a choice-box.

### Memory Management

Choice continuations are allocated from the **heap**:

```c
#define NewChoiceCont(Cont, Arity) \
{ \
  int size = ChoiceContSize(Arity); \
  NEW(Cont, choicecont);    // Bump-pointer allocation \
  // ... additional allocation for arg array \
}
```

They are **not** garbage collected during the lifetime of the choice-box (they're reachable from the choice-box structure).

## Comparison with Prolog

| Feature | Prolog | AGENTS/AKL |
|---------|--------|------------|
| **Choice points** | Stack | Tree (choice-boxes) |
| **Alternatives** | Sequential | Can be concurrent |
| **Trail** | Global stack | Per-context stack |
| **Backtracking** | Chronological | Non-chronological possible |
| **Pruning** | Cut (`!`) | Cut + guards + constraints |
| **State saving** | Choice frames | Choice continuations |

**Prolog** uses a simple **choice stack**:

```
[Choice(p), Choice(q), Choice(r)]
```

Backtracking pops the stack.

**AGENTS** uses a **tree structure**:

```
        Root-Choice
             |
        And-Box(p)
             |
        Choice(q)
        /         \
    And-Box(a)  And-Box(b)
        |
    Choice(r)
    /         \
And-Box(c) And-Box(d)
```

Backtracking navigates the tree, allowing more sophisticated control.

## Summary

AGENTS implements nondeterministic search through:

1. **Choice-boxes**: Represent alternatives (disjunctions)
   - Hold saved state in choice continuations
   - Linked into agent tree structure

2. **TRY/RETRY/TRUST**: Manage clause alternatives
   - TRY: Create choice-box, save state
   - RETRY: Add sibling alternative
   - TRUST: Last alternative, remove continuation

3. **Trail mechanism**: Record variable bindings
   - `TrailRef`: Push binding onto trail
   - `UndoTrail`: Restore bindings on backtracking
   - Per-context trail segments

4. **Context stack**: Save stack pointers at each choice level
   - `PushContext`: Save task/wake/recall/trail pointers
   - `PopContext`: Restore to previous level
   - Supports nested choices

5. **Backtracking**: Restore state and try alternatives
   - Undo trail (restore variables)
   - Restore arguments (from choice continuation)
   - Create new and-box for next clause
   - Execute alternative

**Key data structures:**

- `choicebox`: Holds alternatives and saved state
- `choicecont`: Saved arguments and program counter
- `trailentry`: Recorded variable binding
- `context`: Saved stack pointers

**Key instructions:**

- `TRY label`: Create choice-box, try first clause
- `RETRY label`: Try next clause
- `TRUST label`: Try last clause

**Key algorithms:**

- **Choice creation**: TRY instruction sequence
- **Trail unwinding**: Iterate backwards, restore bindings
- **Backtracking**: UndoTrail, RestoreAreg, execute next clause

This mechanism enables AGENTS to:
- **Search** nondeterministically through multiple solutions
- **Backtrack** efficiently using the trail
- **Commit** to clauses using guards
- **Prune** alternatives using cuts

Combined with the concurrent execution model (Chapter 5), this provides a powerful foundation for:
- **Constraint solving**: Backtracking search with constraint propagation
- **Planning**: Generate and test with backtracking
- **Parsing**: Nondeterministic grammar rules
- **Database queries**: Multiple matching clauses

In the next chapters, we'll explore:
- **Chapter 13**: And-boxes and concurrent agent management
- **Chapter 14**: Constraint variables and suspension
- **Chapter 15**: Port communication

The choice point mechanism is one of the three pillars of AGENTS (along with unification and concurrent execution), enabling the powerful search capabilities of logic programming.

---

**Key source files:**

- `emulator/engine.c:260-370` - TRY/RETRY/TRUST instruction handlers
- `emulator/tree.h:95-118` - Choice-box and continuation structures
- `emulator/exstate.h:16-37,249-353` - Trail and context stack
- `emulator/config.h:234-521` - Choice-box creation and management macros
