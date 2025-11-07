# Chapter 5: The Execution Model

## Introduction

AGENTS implements a sophisticated **concurrent constraint programming** execution model that combines:

1. **And-parallelism**: Multiple agents execute concurrently within a conjunction
2. **Or-parallelism**: Multiple clauses provide nondeterministic alternatives
3. **Guard-based selection**: Guards control which clause commits
4. **Constraint suspension**: Agents suspend when constraints are not satisfied

This execution model goes far beyond traditional Prolog's sequential backtracking search. Instead of a simple choice point stack, AGENTS maintains a **dynamic tree structure** where:

- **And-boxes** represent concurrent agents (conjunctions)
- **Choice-boxes** represent nondeterministic alternatives (disjunctions)
- Agents can **suspend** on constraint variables
- **Guards** control commitment to clauses
- A **task queue** schedules pending operations

The result is a system where multiple agents can execute concurrently, suspend when waiting for constraints, and commit to clauses based on guard evaluation—all while maintaining the logical semantics of concurrent constraint programming.

### Historical Context

This execution model implements the **Andorra Principle**: run deterministic work eagerly, delay nondeterministic choices. The AKL language extends this with:

- **Guards** that synchronize on constraint satisfaction
- **Ports** for asynchronous communication
- **Constraint variables** that suspend agents until bound

The implementation draws from:
- **WAM** (Warren Abstract Machine): environment frames, argument registers
- **GHC** (Guarded Horn Clauses): guard-based commitment
- **Concurrent Prolog**: and-parallelism and stream communication
- **CLP** (Constraint Logic Programming): constraint suspension

## The Agent Tree

At the heart of the execution model is a **dynamic tree structure** that represents the current state of computation:

```
             Root Choice-Box
                   |
          +--------+--------+
          |                 |
      And-Box 1         And-Box 2
          |                 |
    Choice-Box A      Choice-Box B
          |                 |
    +-----+-----+      +----+----+
    |           |      |         |
  And-Box     And-Box And-Box  And-Box
```

### And-Boxes: Concurrent Agents

An **and-box** represents a conjunction of goals—essentially, an agent executing a clause body.

**Definition** (from `emulator/tree.h:66-80`):

```c
typedef enum {
  DEAD,        // Agent has terminated
  STABLE,      // No pending operations
  UNSTABLE,    // Has pending operations
  XSTABLE,     // Extended stable
  XUNSTABLE    // Extended unstable
} sflag;

typedef struct andbox {
  sflag             status;      // Liveness and stability
  struct envid      *env;         // Environment pointer
  struct unifier    *unify;       // Pending unifications
  struct constraint *constr;      // Pending constraints
  struct choicebox  *tried;       // Nested choice-box (or NULL)
  struct andcont    *cont;        // Continuation (code + environment)
  struct choicebox  *father;      // Parent choice-box
  struct andbox     *next;        // Next sibling
  struct andbox     *previous;    // Previous sibling
} andbox;
```

**Key relationships:**

1. **Vertical**: `tried` points to nested choice-boxes
2. **Horizontal**: `next`/`previous` link sibling and-boxes
3. **Upward**: `father` points to parent choice-box
4. **Continuation**: `cont` holds the code pointer and environment (Y registers)

**Status flags:**

- **DEAD**: Agent has terminated (succeeded or failed)
- **STABLE**: No pending unifications or constraints
- **UNSTABLE**: Has pending work (unifications, constraints)
- **XSTABLE/XUNSTABLE**: Extended states (used during promotion)

**Key predicates** (from `emulator/tree.h:3-18`):

```c
#define Quiet(A)   (((A)->unify == NULL) && \
                    ((A)->constr == NULL || entailed(&(A)->constr)))

#define Solved(A)  ((A)->tried == NULL)

#define Leftmost(A) ((A)->previous == NULL)
#define Rightmost(A) ((A)->next == NULL)
```

An agent is **quiet** when it has no pending work. It's **solved** when it has no nested choices. It's **leftmost** when it has no previous siblings.

### Choice-Boxes: Nondeterministic Alternatives

A **choice-box** represents a disjunction—a set of alternative clauses that could be tried.

**Definition** (from `emulator/tree.h:95-107`):

```c
typedef struct choicebox {
  struct choicecont *cont;        // Continuation (saved state)
  struct choicebox  *next;        // Next sibling choice-box
  struct choicebox  *previous;    // Previous sibling
  struct andbox     *father;      // Parent and-box
  struct predicate  *def;         // Predicate being called
  struct andbox     *tried;       // List of alternative and-boxes
  int               type;         // Choice-box type
} choicebox;
```

**Key relationships:**

1. **Vertical**: `tried` points to the list of alternative and-boxes
2. **Horizontal**: `next`/`previous` link sibling choice-boxes
3. **Upward**: `father` points to parent and-box
4. **Continuation**: `cont` holds saved arguments for backtracking

When a goal is called with multiple matching clauses, AGENTS creates:
1. A choice-box to hold the alternatives
2. An and-box for each clause body
3. Links them into the agent tree

### Continuations: Saved State

Two types of continuations preserve execution context:

**And-continuation** (from `emulator/tree.h:82-93`):

```c
typedef struct andcont {
  code           *label;     // Return address (code pointer)
  struct andcont *next;      // Previous frame
  int            ysize;      // Number of Y registers
  Term           yreg[ANY];  // Environment (Y registers)
} andcont;
```

This is the **environment frame** for a procedure call. It holds:
- **label**: Where to return when agent completes
- **next**: Link to caller's environment
- **yreg**: Local variables (permanent variables, Y registers)

**Choice-continuation** (from `emulator/tree.h:111-118`):

```c
typedef struct choicecont {
  union {
    code  *label;      // Next clause to try
    bool  (*cinfo)();  // Or: built-in continuation
  } instr;
  int   arity;         // Number of arguments
  Term  arg[ANY];      // Saved argument registers
} choicecont;
```

This holds the **saved state for backtracking**:
- **label**: Code pointer to next alternative
- **arg**: Saved argument registers (X registers)

Both structures are variable-sized, allocated from the heap.

### The Agent Tree in Action

Consider the AKL program:

```prolog
p(X) := q(X), r(X).
q(1).
q(2).
r(X) := X > 0.
```

When `p(X)` is called, AGENTS creates:

```
         Root Choice-Box
               |
         And-Box (p/1)
               |
         Choice-Box (q/1)
          |           |
    And-Box(q=1)  And-Box(q=2)
          |           |
    [r(1) goal]   [r(2) goal]
```

Both `q(1)` and `q(2)` can execute **concurrently** as sibling and-boxes. This is the essence of **and-parallelism** in AKL.

## Agent States and Lifecycle

### State Transitions

An and-box transitions through several states during its lifetime:

```
   UNSTABLE ←→ STABLE ←→ DEAD
       ↓           ↓
   XUNSTABLE   XSTABLE
```

1. **Creation**: Agent starts as UNSTABLE (has pending work)
2. **Stabilization**: Becomes STABLE when all unifications/constraints done
3. **Promotion/Demotion**: Transitions through X-states during tree restructuring
4. **Termination**: Becomes DEAD when completed

**State checking macros** (from `emulator/tree.h:24-49`):

```c
#define Dead(Andb)     ((Andb)->status == DEAD)
#define Live(Andb)     ((Andb)->status != DEAD)
#define Stable(Andb)   ((Andb)->status == STABLE || \
                        (Andb)->status == XSTABLE)
#define UnStable(Andb) ((Andb)->status == UNSTABLE || \
                        (Andb)->status == XUNSTABLE)

#define ResetState(A)  A->status += 2   // STABLE → XSTABLE
#define SetState(A)    A->status -= 2   // XSTABLE → STABLE
```

The **extended states** (XSTABLE, XUNSTABLE) are used during **promotion** and **demotion** when the agent tree is restructured. They mark agents that are in transition.

### Agent Operations

**Pending work queues:**

1. **Unification queue** (`andb->unify`):
   - Pending unifications to perform
   - Each entry holds two terms to unify
   - Processed when agent becomes active

2. **Constraint queue** (`andb->constr`):
   - Pending constraints to install
   - Each constraint has methods: install, reinstall, deinstall
   - Checked for entailment

**Definition** (from `emulator/tree.h:135-139`):

```c
typedef struct unifier {
  struct unifier *next;
  Term           arg1;
  Term           arg2;
} unifier;
```

When an agent performs work, it:
1. Executes bytecode from `cont->label`
2. Processes pending unifications
3. Installs/checks constraints
4. May create nested choice-boxes
5. Eventually stabilizes or terminates

## The Scheduling Loop

The core of AGENTS execution is the **choicehandler** in `emulator/engine.c:3386-3568`. This is the main scheduling loop that:

1. Processes the task queue
2. Selects agents to run
3. Handles choice points
4. Manages guards

### The Task Queue

The task queue is a **stack** of pending operations that need to be performed.

**Definition** (from `emulator/exstate.h:300-329`):

```c
typedef enum {
  CHB,       // Choice-box task
  ANDB,      // And-box task
  UNIFY_TASK,
  // ... other types
} tasktype;

typedef struct task {
  tasktype  type;
  union {
    andbox     *andbox;
    choicebox  *choicebox;
    // ... other fields
  } info;
} task;

typedef struct taskhead {
  int   size;
  task  *start;
  task  *current;    // Stack pointer
  task  *end;
} taskhead;
```

**Task queue macros** (from `emulator/exstate.h:5-11`):

```c
#define NextTask(W,T)   {T = (--(W)->task.current);}
#define EmptyTask(W)    ((W)->task.current == (W)->context.current->task)
```

The task queue is a **stack** (not a queue!): `current` decrements to pop tasks.

### The choicehandler Loop

The main scheduling loop (from `emulator/engine.c:3386-3568`):

```c
choicehandler:
{
  task *entry;

  // Process all pending tasks
  while(!EmptyTask(exs)) {
    NextTask(exs, entry);

    switch(entry->type) {
      case ANDB:
        // Activate an and-box
        andb = entry->info.andbox;
        chb = andb->father;
        pc = andb->cont->label;
        FetchOpCode(0);
        yreg = andb->cont->yreg;
        NextOp();    // Jump to first instruction

      case CHB:
        // Backtrack to choice-box
        // ... restore state, try next alternative

      // ... other task types
    }
  }

  // Task queue empty: try to find work
  if(NonEmptyChoiceCont(chb)) {
    // Choice-box has more alternatives
    // ... create new and-box for next clause
  }

  // If no work available, search for candidate to promote
  // ... (candidate selection)

  goto choicehandler;   // Loop forever
}
```

**Control flow:**

1. **Task processing**: While tasks exist, pop and execute them
2. **Choice handling**: If current choice-box has alternatives, try them
3. **Candidate selection**: Find and promote suspended agents
4. **Repeat**: Loop back to process more tasks

The choicehandler **never returns**. Execution continues until:
- The root goal succeeds/fails
- An exception is thrown
- The system halts

### Dispatching to Agents

When an ANDB task is popped, the system:

1. **Sets current and-box**: `andb = entry->info.andbox`
2. **Restores parent choice-box**: `chb = andb->father`
3. **Sets program counter**: `pc = andb->cont->label`
4. **Restores environment**: `yreg = andb->cont->yreg`
5. **Jumps to instruction**: `NextOp()`

This **activates** the agent, transferring control to its bytecode.

Agents execute instructions until they:
- **Suspend** (on a guard or constraint)
- **Complete** (reach PROCEED)
- **Call** another goal (creating new agents)
- **Backtrack** (create a CHB task)

When an agent suspends, control returns to the choicehandler via `goto choicehandler`.

## Guards and Commitment

Guards are the key to AKL's **concurrent execution model**. They control when a clause **commits** (becomes the chosen alternative).

### Guard Types

AKL supports three types of guards (from `emulator/engine.c:374-535`):

1. **GUARD_UNIT**: Leftmost guard, commits immediately when quiet
2. **GUARD_ORDER**: Ordered guard, must be leftmost to commit
3. **GUARD_UNORDER**: Unordered guard, can commit when quiet and solved

**GUARD_UNIT** (from `emulator/engine.c:489-535`):

```c
CaseLabel(GUARD_UNIT):
  /* If the guard is quiet and leftmost then we should promote
   * the agent to the parent context.
   */
  if(Quiet(andb) && EmptyTrail(exs) && Leftmost(andb)) {
    // Promote immediately
    promote(andb, exs);
    andb = exs->andb;
    chb = andb->father;
    pc = andb->cont->label;
    FetchOpCode(0);
    yreg = andb->cont->yreg;
    NextOp();
  }
  goto guardhandler;
```

When the guard is **quiet** (no pending work), **leftmost** (no earlier alternatives), and the **trail is empty** (no backtrackable bindings), it **promotes**.

**GUARD_ORDER** (from `emulator/engine.c:432-487`):

```c
CaseLabel(GUARD_ORDER):
  if(Quiet(andb) && EmptyTrail(exs) && Solved(andb) && Leftmost(andb)) {
    // Promote if solved (no nested choices)
    promote(andb, exs);
    andb = exs->andb;
    chb = andb->father;
    pc = andb->cont->label;
    FetchOpCode(0);
    yreg = andb->cont->yreg;
    NextOp();
  }
  goto guardhandler;
```

Similar to GUARD_UNIT, but also requires **Solved** (no nested choice-boxes).

**GUARD_UNORDER** (from `emulator/engine.c:374-430`):

```c
CaseLabel(GUARD_UNORDER):
  if(Quiet(andb) && EmptyTrail(exs) && Solved(andb)) {
    // Can commit even if not leftmost
    promote(andb, exs);
    andb = exs->andb;
    chb = andb->father;
    pc = andb->cont->label;
    FetchOpCode(0);
    yreg = andb->cont->yreg;
    NextOp();
  }
  goto guardhandler;
```

Does **not** require leftmost—can commit from any position if conditions met.

### The guardhandler

When a guard cannot commit (not quiet, or not leftmost for ordered guards), control jumps to `guardhandler`:

```c
guardhandler:
  // Find another agent to run
  // ... traverse siblings, try other choice-boxes
  // ... eventually goto choicehandler
```

The guardhandler searches for other work:
1. Try sibling and-boxes in the same choice-box
2. Move to parent and-boxes
3. Eventually return to choicehandler

This implements **suspension**: guards that can't commit suspend, allowing other agents to run.

### Promotion: Committing to a Clause

When a guard commits, **promotion** occurs (details in later chapters). Conceptually:

1. **Remove siblings**: Kill alternative and-boxes in same choice-box
2. **Lift to parent**: Move committed and-box to parent's level
3. **Continue execution**: Resume executing the committed clause body

This is the **commitment** operation in guarded concurrent programming.

## Candidate Selection

When no agents are ready to run (all suspended on guards), AGENTS searches for a **candidate** to promote nondeterministically.

### The Candidate Function

From `emulator/candidate.c:51-55`:

```c
andbox *candidate(choicebox *chb)
{
  return leftmost(chb);
}
```

The candidate selection strategy is **leftmost**: find the leftmost suspended agent.

### Leftmost Strategy

The `leftmost()` function (from `emulator/candidate.c:60-180`) implements depth-first leftmost search:

```c
andbox *leftmost(choicebox *chb)
{
  andbox *ab, *deep, *cand;
  choicebox *cb;

  cand = NULL;
  cb = chb;

nxtcb:
  ab = cb->tried;    // Start with first and-box

nxtab:
  if(ab == NULL)
    goto endab;

  // Check guard type
  if(GetOpCode(ab->cont->label) == EnumToCode(GUARD_WAIT)) {
    if(ab->tried == NULL) {
      cand = ab;       // Found a candidate
      goto found;
    }
    // Recurse into nested choice-boxes
    deep = leftmost(ab->tried);
    if(deep != NULL)
      return deep;
    ab = ab->next;     // Try next sibling
    goto nxtab;
  }

  // ... handle other guard types similarly

endab:
  // No candidate found at this level
  return NULL;

found:
  return cand;
}
```

**Algorithm:**

1. Traverse and-boxes **left to right** (via `next` pointers)
2. For each and-box with a GUARD_WAIT instruction:
   - If it has no nested choices (`tried == NULL`), it's a candidate
   - Otherwise, **recurse** into nested choice-boxes
3. Return the **first** (leftmost) candidate found

This implements the **Andorra Principle**: prefer deterministic computation, delay nondeterministic choices.

### Other Guard Types

The leftmost search handles various guard types:

- **GUARD_WAIT**: Suspended, waiting for promotion
- **GUARD_CUT**: Cut operation (prune alternatives)
- **GUARD_COND**: Conditional guard
- **GUARD_COMMIT**: Explicit commitment
- **GUARD_UNORDER/GUARD_ORDER/GUARD_UNIT**: Normal guards

Each has specific rules for whether it's a valid candidate.

## Example Execution

Let's trace a simple example to see the execution model in action.

### Example: Concurrent Queries

```prolog
% AKL program
p :- q & r.      % Concurrent conjunction
q :- X = 1.      % Bind X to 1
r :- Y = 2.      % Bind Y to 2
```

**Initial state:**

```
Root Choice-Box
      |
And-Box(p) [UNSTABLE]
  cont: label → "q & r" code
```

**Step 1**: Execute `p`, encounter `q & r`:

```
Root Choice-Box
      |
And-Box(p) [UNSTABLE]
      |
Choice-Box
      |
And-Box(q & r) [UNSTABLE]
  cont: label → call q, call r
```

**Step 2**: Call `q`, creating new and-box:

```
Root Choice-Box
      |
And-Box(p)
      |
Choice-Box
      |
And-Box(q & r)
      |
Choice-Box(q)
      |
And-Box(X=1) [UNSTABLE]
```

**Step 3**: Execute `X=1`, unification succeeds:

```
And-Box(X=1) [STABLE]    % X now bound to 1
```

**Step 4**: Agent `X=1` completes, reaches PROCEED. Control returns to choicehandler.

**Step 5**: Call `r`, creating sibling and-box:

```
Choice-Box(q)
      |
And-Box(X=1) [DEAD]   And-Box(Y=2) [UNSTABLE]
```

Both `X=1` and `Y=2` exist as **siblings**—they can execute **concurrently**.

**Step 6**: Execute `Y=2`, bind Y to 2:

```
And-Box(Y=2) [STABLE → DEAD]
```

**Step 7**: Both agents complete. The conjunction `q & r` succeeds. Promotion occurs, lifting the result to parent.

**Final state**: Goal `p` succeeds.

### Key Observations

1. **Concurrent execution**: `q` and `r` create sibling and-boxes
2. **Task queue**: Each goal creates an ANDB task
3. **Choicehandler**: Dispatches agents from task queue
4. **State transitions**: UNSTABLE → STABLE → DEAD
5. **Promotion**: Successful guards promote to parent

This example shows how AGENTS achieves **and-parallelism**: multiple goals in a conjunction become sibling and-boxes that can run concurrently.

## Comparison with Prolog

Traditional Prolog has a much simpler execution model:

| Feature | Prolog | AGENTS/AKL |
|---------|--------|------------|
| **Conjunction** | Sequential (`,`) | Concurrent (`&`) |
| **Choice points** | Stack | Tree (choice-boxes) |
| **Backtracking** | Chronological | Non-chronological |
| **Commitment** | Cut (`!`) | Guards (`→`, `|`, `?`) |
| **Suspension** | None | Constraint variables |
| **Parallelism** | None | And-parallelism + or-parallelism |

**Prolog execution:**

```
Goal stack:    [g1, g2, g3]
Choice stack:  [cp1, cp2]
Trail:         [X=1, Y=2]
```

Simple stacks, sequential execution.

**AGENTS execution:**

```
Agent tree:
    Root Choice-Box
         |
    +----+----+
    |         |
  And-Box  And-Box   (concurrent)
    |         |
   ...       ...

Task queue: [ANDB(a1), CHB(c1), ...]
Wake queue: [suspended agents]
Trail:      [bindings for backtracking]
```

Complex tree structure, concurrent execution, multiple queues.

## Implementation Details

### Register Allocation

The choicehandler maintains key pointers in register variables (from `emulator/engine.c:139-154`):

```c
register andbox    *andb REGISTER2;     // Current and-box
register choicebox *chb = NULL;         // Current choice-box
register choicebox *insert;             // Insertion point for choices
```

These are the **execution context**. When switching agents:

```c
andb = new_agent;              // Switch to new agent
chb = andb->father;            // Set parent choice-box
yreg = andb->cont->yreg;       // Restore Y registers
pc = andb->cont->label;        // Set program counter
```

### Exception Handling

The choicehandler also handles exceptions (from `emulator/engine.c:87-103`):

```c
#define CheckExceptions \
{\
  if(interruptflag) {\
    interruptflag = NO_INTRPT;\
    longjmp(toplevenv, 1);\
  }\
  if(gcflag) {\
    exs->arity = arity;\
    exs->andb = andb;\
    exs->insert = insert;\
    gc(exs);                    // Run garbage collection
    andb = exs->andb;\
    insert = exs->insert;\
    chb = andb->father;\
    yreg = andb->cont->yreg;\
  }\
}
```

**Interrupt flag**: User interrupt (Ctrl-C)
**GC flag**: Heap full, need garbage collection

These are checked periodically during execution.

### Context Switching

AGENTS supports multiple **execution states** (workers). Context switching between them involves:

```c
#define SaveRegisters \
{\
  exs->andb = andb;\
  exs->insert = insert;\
}

#define RestoreRegisters \
{\
  andb = exs->andb;\
  insert = exs->insert;\
  chb = andb->father;\
  yreg = andb->cont->yreg;\
}
```

This allows **multiprocessing**: multiple independent AGENTS computations in the same process.

## Summary

The AGENTS execution model is a sophisticated implementation of concurrent constraint programming:

1. **Agent tree**: Dynamic tree of and-boxes (agents) and choice-boxes (alternatives)

2. **And-parallelism**: Sibling and-boxes execute concurrently

3. **Or-parallelism**: Choice-boxes provide nondeterministic alternatives

4. **Guards**: Control commitment to clauses based on constraint satisfaction

5. **Task queue**: Schedules pending agent activations

6. **Choicehandler**: Main scheduling loop that dispatches agents

7. **Candidate selection**: Leftmost strategy for nondeterministic promotion

8. **Suspension**: Agents suspend on guards, waiting for constraints to be satisfied

9. **Promotion**: Successful guards commit, restructuring the agent tree

**Key data structures:**

- `andbox`: Concurrent agent with status, continuation, work queues
- `choicebox`: Nondeterministic alternative with saved state
- `andcont`: Environment frame (Y registers, return address)
- `choicecont`: Saved arguments for backtracking
- `task`: Queue entry for pending operations

**Key algorithms:**

- **choicehandler**: Task queue processing, agent dispatching
- **guardhandler**: Guard evaluation, commitment, suspension
- **leftmost**: Candidate selection for nondeterministic promotion
- **promote**: Commitment operation (restructure tree)

This execution model enables AKL to express:
- **Concurrent algorithms** (producer-consumer, pipeline)
- **Nondeterministic search** (N-Queens, graph coloring)
- **Constraint solving** (finite domains, scheduling)
- **Synchronization** (guards, ports)

All while maintaining the **logical semantics** of logic programming.

In the next chapters, we'll explore:
- **Chapter 10**: Instruction dispatch mechanisms (threaded code)
- **Chapter 12**: Choice points and backtracking in detail
- **Chapter 13**: And-boxes and concurrent agent management
- **Chapter 14**: Constraint variables and suspension
- **Chapter 15**: Port communication

The execution model is the foundation for understanding how AGENTS achieves its unique combination of logic programming, concurrency, and constraints.

---

**Key source files:**

- `emulator/tree.h:66-158` - And-box and choice-box definitions
- `emulator/engine.c:3386-3568` - choicehandler scheduling loop
- `emulator/candidate.c:60-180` - Leftmost candidate selection
- `emulator/exstate.h:300-442` - Task queue and execution state
