# Chapter 13: And-Boxes and Concurrent Agent Execution

## Introduction

In AGENTS, an **and-box** is the fundamental unit of concurrent execution. While traditional Prolog executes goals sequentially, AGENTS allows multiple goals to execute **concurrently** as independent agents.

This chapter explores:

1. **And-box lifecycle**: Creation, execution, stabilization, termination
2. **Concurrent execution**: How sibling and-boxes run in parallel
3. **Agent management**: State transitions, pending work queues
4. **Promotion and demotion**: Restructuring the agent tree
5. **Sequential vs concurrent conjunction**: The difference between `,` and `&`

The and-box mechanism is what makes AGENTS a **concurrent** constraint programming system, not just a logic programming system. It enables:

- **And-parallelism**: Multiple goals executing simultaneously
- **Stream communication**: Producer-consumer patterns via ports
- **Nondeterministic search**: Exploring multiple branches concurrently
- **Constraint propagation**: Suspended agents waking when variables bind

## And-Box Structure

### Definition

An and-box represents a **conjunction of goals**—essentially, an agent executing a clause body (from `emulator/tree.h:66-80`):

```c
typedef enum {
  DEAD,        // Agent has terminated
  STABLE,      // No pending operations
  UNSTABLE,    // Has pending operations
  XSTABLE,     // Extended stable (during tree restructuring)
  XUNSTABLE    // Extended unstable (during tree restructuring)
} sflag;

typedef struct andbox {
  sflag             status;      // Liveness and stability status
  struct envid      *env;         // Environment pointer
  struct unifier    *unify;       // Pending unifications queue
  struct constraint *constr;      // Pending constraints queue
  struct choicebox  *tried;       // Nested choice-boxes
  struct andcont    *cont;        // Continuation (code + environment)
  struct choicebox  *father;      // Parent choice-box
  struct andbox     *next;        // Next sibling and-box
  struct andbox     *previous;    // Previous sibling and-box
} andbox;
```

### Key Fields

**Status**: Current state of the agent
- **DEAD**: Agent has completed (successfully or failed)
- **STABLE**: Agent has no pending work, ready to execute bytecode
- **UNSTABLE**: Agent has pending unifications or constraints to process
- **XSTABLE/XUNSTABLE**: Temporary states during promotion/demotion

**Work queues**:
- **unify**: Linked list of pending unifications
- **constr**: Linked list of pending constraints

**Tree structure**:
- **father**: Points upward to parent choice-box
- **tried**: Points downward to nested choice-boxes (created by this agent)
- **next/previous**: Links horizontally to sibling agents

**Continuation**:
- **cont**: Points to the and-continuation (code pointer + Y registers)

This structure embeds the and-box into a **dynamic tree**:

```
        Choice-Box (father)
             |
    +--------+--------+--------+
    |        |        |        |
  And-Box  And-Box  And-Box  And-Box   (siblings via next/previous)
  (prev)   (this)   (next)
    |        |
  Choice   Choice                       (nested via tried)
```

### And-Continuation

The **and-continuation** holds the execution context for an and-box (from `emulator/tree.h:82-93`):

```c
typedef struct andcont {
  code           *label;     // Program counter (where to execute/return)
  struct andcont *next;      // Previous continuation (caller's frame)
  int            ysize;      // Number of Y registers (local variables)
  Term           yreg[ANY];  // Environment (Y registers)
} andcont;
```

**Purpose**:
- **label**: Where the agent should execute (or return to when it completes)
- **yreg**: Local variables (permanent variables, Y registers)
- **next**: Forms a linked stack of continuations (call stack)

This is analogous to a **stack frame** in traditional imperative languages, but allocated on the heap and linked explicitly.

**Size calculation**:

```c
#define AndContSize(Arity) \
    (sizeof(andcont) + sizeof(Term) * ((Arity) - ANY))
```

The continuation is **variable-sized** based on how many Y registers are needed.

## And-Box Lifecycle

### Creation

And-boxes are created by several instructions:

1. **TRY/RETRY/TRUST**: Create and-boxes for alternative clauses
2. **CALL**: Implicitly creates continuation for called predicate
3. **Root**: Create initial and-box for top-level goal

**Macro** (from `emulator/config.h:158-184`):

```c
#define MakeAndbox(Andb, Chb) \
{ \
  NewAndbox(Andb);               // Allocate from heap \
  Andb->env = NULL; \
  Andb->status = STABLE;         // Start stable \
  Andb->tried = NULL;            // No nested choices yet \
  Andb->cont = NULL;             // No continuation yet \
  Andb->constr = NULL;           // No constraints yet \
  Andb->unify = NULL;            // No pending unifications \
  Andb->father = (Chb);          // Set parent choice-box \
  Andb->previous = NULL; \
  Andb->next = NULL; \
}
```

**Allocation** (from `emulator/storage.h:141-151`):

```c
#define NewAndbox(ret) \
{ \
  (ret) = (andbox *) heapcurrent; \
  heapcurrent += sizeof(andbox); \
  if(heapcurrent >= heapend) { \
    SaveHeapRegisters(); \
    reinit_heap((ret), sizeof(andbox)); \
    RestoreHeapRegisters(); \
    (ret) = (andbox *) heapcurrent; \
    heapcurrent += sizeof(andbox); \
  } \
}
```

And-boxes are allocated using **bump-pointer allocation** from the heap. They're garbage collected when no longer reachable.

### Initialization

After creation, the and-box is initialized:

```c
// From TRY instruction (emulator/engine.c:307):
MakeAndbox(andb, chb);      // Create and-box
chb->tried = andb;          // Link to choice-box
```

Or for RETRY/TRUST (engine.c:337, 361):

```c
MakeAndbox(andb, chb);
InsertAndbox(andb, prev, chb);   // Insert as sibling
```

**InsertAndbox** macro inserts the new and-box into the doubly-linked list of siblings:

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

This creates a **sibling list**:

```
Choice-Box
    |
And-Box₁ ←→ And-Box₂ ←→ And-Box₃
(leftmost)              (rightmost)
```

### Execution

Once created, an and-box is activated by:

1. **Task queue**: An ANDB task is pushed onto the task queue
2. **Choicehandler**: The task is popped and executed
3. **Dispatch**: Control transfers to the and-box's code

**From choicehandler** (engine.c:3393-3402):

```c
switch(entry->type) {
  case ANDB:
    // Activate an and-box
    andb = entry->info.andbox;     // Set current and-box
    chb = andb->father;            // Set parent choice-box
    pc = andb->cont->label;        // Set program counter
    FetchOpCode(0);                // Fetch first instruction
    yreg = andb->cont->yreg;       // Set Y registers
    NextOp();                      // Execute!
```

This **context switch** transfers control to the and-box. The agent then executes bytecode instructions until:

- It **completes** (reaches PROCEED)
- It **suspends** (on a guard or constraint)
- It **calls** another goal (creating new agents)
- It **fails** (backtracking)

### State Transitions

An and-box transitions through several states during execution:

```
  [Created]
      |
      v
  STABLE ←→ UNSTABLE
      |         |
      v         v
  XSTABLE   XUNSTABLE   (during promotion/demotion)
      |         |
      v         v
    DEAD      DEAD
```

**STABLE → UNSTABLE**: When unifications or constraints are added to work queues

**UNSTABLE → STABLE**: After processing all pending work

**STABLE → XSTABLE**: During promotion (tree restructuring)

**Any → DEAD**: When agent completes or fails

**State checking macros** (from `emulator/tree.h:24-49`):

```c
#define Dead(Andb)     ((Andb)->status == DEAD)
#define Live(Andb)     ((Andb)->status != DEAD)

#define Stable(Andb)   ((Andb)->status == STABLE || \
                        (Andb)->status == XSTABLE)

#define UnStable(Andb) ((Andb)->status == UNSTABLE || \
                        (Andb)->status == XUNSTABLE)

#define Quiet(A)       (((A)->unify == NULL) && \
                        ((A)->constr == NULL || entailed(&(A)->constr)))

#define Solved(A)      ((A)->tried == NULL)
```

An agent is **quiet** when it has no pending work. It's **solved** when it has no nested choice-boxes.

### Termination

An and-box terminates when:

1. **Success**: Reaches PROCEED instruction
2. **Failure**: Unification fails or explicit failure
3. **Commitment**: Guard commits, killing siblings

When an and-box terminates, its status becomes **DEAD**:

```c
andb->status = DEAD;
```

Dead and-boxes remain in the tree structure (for backtracking) but won't execute further.

## Concurrent Execution

### And-Parallelism

The key to AGENTS' concurrency is that **sibling and-boxes can execute concurrently**.

**Example AKL program**:

```prolog
p :- q, r.
q.
r.
```

**Sequential execution** (Prolog):
1. Call `q`
2. Wait for `q` to complete
3. Call `r`
4. Wait for `r` to complete

**Concurrent execution** (AGENTS):
1. Call `q`, creating and-box₁
2. Call `r`, creating and-box₂ (sibling of and-box₁)
3. Both execute **concurrently**
4. Both complete (in any order)

### Concurrent Conjunction (`&`)

AKL supports **explicit concurrent conjunction** with the `&` operator:

```prolog
p :- q & r.   % q and r execute concurrently
```

This compiles to code that creates sibling and-boxes directly.

**Contrast with sequential conjunction** (`,`):

```prolog
p :- q, r.    % q executes, then r
```

This compiles to:
1. CALL q (creates and-box, waits for completion)
2. CALL r (creates and-box)

The difference is **timing**: `&` creates both and-boxes immediately; `,` waits for the first to complete.

### Task Queue and Scheduling

The **task queue** manages pending agent activations:

```c
typedef struct task {
  tasktype  type;       // ANDB, CHB, PROMOTE, etc.
  union {
    andbox  *andbox;
    code    *code;
  } value;
} task;
```

When an and-box is ready to run, an **ANDB task** is pushed:

```c
// From engine.c or elsewhere:
exs->task.current->type = ANDB;
exs->task.current->value.andbox = new_agent;
exs->task.current++;
```

The choicehandler pops tasks and dispatches agents:

```c
while(!EmptyTask(exs)) {
  NextTask(exs, entry);
  switch(entry->type) {
    case ANDB:
      // Activate agent
      andb = entry->info.andbox;
      // ... set pc, yreg, etc.
      NextOp();   // Execute
  }
}
```

This implements **cooperative multitasking**: agents yield control by returning to the choicehandler, which schedules the next agent.

### Fairness

AGENTS doesn't guarantee **fairness** (all agents eventually execute). The scheduling strategy is:

1. **Task queue order**: LIFO (stack)
2. **Candidate selection**: Leftmost first

In practice, this works well for most programs, but pathological cases could lead to starvation.

## Work Queues

And-boxes maintain two work queues for pending operations:

### Unifier Queue

**Definition** (from `emulator/tree.h:135-139`):

```c
typedef struct unifier {
  struct unifier *next;
  Term           arg1;
  Term           arg2;
} unifier;
```

**Purpose**: Store pairs of terms that need to be unified.

**Example**: When a guard suspends, unifications may be **deferred** rather than performed immediately. They're added to the unifier queue and processed when the guard succeeds.

**Processing**: When an agent becomes active, pending unifications are performed:

```c
// Simplified:
while(andb->unify != NULL) {
  unifier *u = andb->unify;
  bool success = unify(u->arg1, u->arg2, andb, exs);
  if(!success) {
    // Unification failed → agent fails
    goto fail;
  }
  andb->unify = u->next;   // Remove from queue
}
```

### Constraint Queue

**Definition** (from `emulator/tree.h:120-133`):

```c
typedef struct constrtable {
  bool  (*install)();       // Install constraint
  void  (*reinstall)();     // Reinstall after backtracking
  void  (*deinstall)();     // Remove constraint
  void  (*promote)();       // Promote to parent context
  int   (*print)();         // Print for debugging
  struct constraint *(*copy)();   // Copy for promotion
  struct constraint *(*gc)();     // Garbage collect
} constrtable;

typedef struct constraint {
  constrtable       *method;      // Method table
  struct constraint *next;        // Next constraint in queue
} constraint;
```

**Purpose**: Store constraints that need to be installed on variables.

**Finite domain constraints**: For example, `X in 1..10` creates a constraint object that:
- Narrows the domain of `X` to {1, 2, ..., 10}
- Suspends the agent if needed
- Wakes the agent when `X` is bound

**Processing**: When an agent becomes active, constraints are installed:

```c
// Simplified:
while(andb->constr != NULL) {
  constraint *c = andb->constr;
  bool success = c->method->install(c, andb, exs);
  if(!success) {
    // Constraint unsatisfiable → agent fails
    goto fail;
  }
  andb->constr = c->next;
}
```

## Promotion and Demotion

When a guard **commits**, the successful agent is **promoted** to replace its parent. This restructures the agent tree.

### Promotion Overview

**Scenario**: An agent inside a guarded choice succeeds, committing to that choice.

```
     Father And-Box
           |
     Choice-Box
      /    |    \
  Andb₁ Andb₂ Andb₃   (alternatives)
          |
     (Andb₂ succeeds)
```

**After promotion**:

```
     Father And-Box
           |
     (Andb₂ promoted here)
     (Andb₁ and Andb₃ killed)
```

### Promotion Implementation

Promotion is implemented in `emulator/copy.c` (~600 lines). The algorithm is complex because it must:

1. **Copy the successful agent** and its context to the parent level
2. **Kill sibling agents** (alternatives)
3. **Update all references** (variables, continuations, constraints)
4. **Preserve sharing** (circular structures, shared variables)

**Simplified outline**:

```c
void promote(andbox *cand, exstate *exs)
{
  choicebox *fork = cand->father;      // Parent choice-box
  andbox *mother = fork->father;       // Grandparent and-box

  // 1. Unlink candidate from siblings
  if(cand->previous != NULL)
    cand->previous->next = cand->next;
  else
    fork->tried = cand->next;

  if(cand->next != NULL)
    cand->next->previous = cand->previous;

  // 2. Copy candidate's subtree to mother's level
  andbox *copy = copy_tree(cand, mother, exs);

  // 3. Kill remaining siblings (alternatives)
  andbox *sib = fork->tried;
  while(sib != NULL) {
    sib->status = DEAD;
    sib = sib->next;
  }

  // 4. Update execution state
  exs->andb = copy;

  // 5. Remove choice-box
  fork->cont = NULL;   // No more alternatives
}
```

The actual implementation is much more complex, involving:
- **Scavenging algorithm** to copy terms
- **Mark bits** to detect already-copied structures
- **Suspension lists** to update constraint suspensions
- **Context management** to preserve trail, task queue, etc.

### State Transitions During Promotion

During promotion, agents transition through **extended states**:

```c
#define ResetState(A)  A->status += 2   // STABLE → XSTABLE
#define SetState(A)    A->status -= 2   // XSTABLE → STABLE
```

This marks agents that are **in transition**, preventing them from being executed or garbage collected prematurely.

### Demotion (Split)

**Demotion** (or **split**) is the opposite: moving an agent **down** into a new choice-box when creating nondeterministic alternatives.

This is less common but occurs when:
- An agent creates a new choice point dynamically
- The agent tree needs to be restructured for constraint solving

## Sequential vs Concurrent Calls

### CALL Instruction

The **CALL** instruction creates a new execution context (from `emulator/engine.c:682-785`):

```c
CaseLabel(CALL):
  // Fetch predicate to call
  Instr_Pred(CALL, def);

call_definition:
  // Save program counter in continuation (where to return)
  andb->cont->label = pc;
  cont_pc = pc;

  // Dispatch based on predicate type
  switch(def->enter) {
    case ENTER_COMPILED:
      // Jump to compiled code
      pc = def->code;
      break;

    case ENTER_TRUE:
      // Built-in true/0 → just succeed
      pc = cont_pc;
      break;

    case ENTER_INTERPRETED:
      // Call interpreted predicate
      // ... create choice-boxes, etc.
      break;

    case ENTER_C:
      // Call C function
      if(!(def->cinfo(areg, arity, andb, exs))) {
        goto fail;
      }
      pc = cont_pc;
      break;
  }

  FetchOpCode(0);
  NextOp();
```

**Key point**: CALL does **not** create a new and-box. It reuses the current and-box, saving the return address in the continuation.

**This is sequential execution**: The called goal runs in the same agent as the caller.

### Creating Concurrent Agents

To create **concurrent agents**, you need:

1. **Guard clauses** with multiple alternatives (TRY/RETRY/TRUST creates siblings)
2. **Explicit concurrent goals** (via built-in predicates or compiler transformations)
3. **Port communication** (sender and receiver as separate agents)

For example, the AKL compiler transforms:

```prolog
p :- q & r.
```

Into bytecode that creates two sibling and-boxes, one for `q` and one for `r`.

## And-Box Management

### Garbage Collection

And-boxes are **heap-allocated** and subject to garbage collection. They're reachable if:

- Linked from the execution state (`exs->andb`)
- Linked from the root choice-box (`exs->root->tried`)
- Referenced from the task queue

During GC, and-boxes are **copied** to the new semispace (from `emulator/gc.c:869-890`):

```c
// Simplified:
NewAndbox(andb);    // Allocate in new space
andb->status = templa->status;
andb->father = new;
andb->env = NULL;
andb->cont = NULL;   // Continuations copied separately
// ... copy other fields
```

### Memory Overhead

Each and-box is **88 bytes** on 64-bit platforms (with TRACE disabled):

```
sizeof(andbox) = sizeof(sflag) + 7 * sizeof(pointer) + sizeof(struct envid*)
               = 4 + 7*8 + 8
               = 68 bytes (without TRACE fields)
               = ~88 bytes (with alignment and TRACE)
```

For programs with many concurrent agents, this can add up. However:

- Dead and-boxes can be garbage collected
- The heap is typically large (megabytes)
- Most programs have tens to hundreds of concurrent agents, not thousands

## Examples

### Example 1: Concurrent Producer-Consumer

```prolog
% Producer sends numbers to consumer via port P
producer(P, N) :-
  N > 0,
  send(P, N),
  N1 is N - 1,
  producer(P, N1).
producer(_, 0).

% Consumer receives and processes numbers
consumer(P) :-
  receive(P, X),
  process(X),
  consumer(P).
```

**Execution**:
1. `producer/2` and `consumer/1` are called concurrently (as sibling and-boxes)
2. Producer sends messages to port `P`
3. Consumer receives from port `P`
4. Both execute independently, synchronized via the port

### Example 2: Parallel Map

```prolog
% Apply F to each element of list in parallel
pmap(_, [], []).
pmap(F, [X|Xs], [Y|Ys]) :-
  apply(F, X, Y) &           % Apply F to X concurrently
  pmap(F, Xs, Ys).           % Recurse concurrently
```

**Execution**:
- Each recursive call creates a new and-box
- All `apply(F, X, Y)` calls execute concurrently
- Results are assembled into output list

This achieves **data parallelism** through concurrent recursion.

### Example 3: Nondeterministic Search

```prolog
% Find a solution to N-Queens
queens(N, Solution) :-
  range(1, N, Rows),
  permutation(Rows, Solution),
  safe(Solution).

% Each permutation tries concurrently
```

**Execution**:
- `permutation/2` creates choice-boxes with multiple alternatives
- Each alternative is a sibling and-box
- All alternatives can be explored concurrently
- First to succeed commits (via guard)

## Performance Considerations

### Benefits of And-Parallelism

1. **Concurrency**: Multiple agents execute simultaneously
2. **Latency hiding**: While one agent suspends, others continue
3. **Resource utilization**: Keep CPU busy with available work

### Costs

1. **Memory overhead**: Each and-box is ~88 bytes
2. **Context switching**: Overhead of task queue management
3. **Garbage collection**: More heap allocations → more GC pressure

### When to Use Concurrency

**Good use cases**:
- **Stream processing**: Producer-consumer patterns
- **Nondeterministic search**: Explore multiple branches
- **Constraint solving**: Propagate constraints concurrently

**Poor use cases**:
- **Sequential algorithms**: No benefit, just overhead
- **Fine-grained parallelism**: Overhead dominates
- **Tight loops**: Better to vectorize or optimize sequentially

## Summary

And-boxes are the fundamental unit of concurrent execution in AGENTS:

1. **Structure**: Hold status, work queues, continuation, tree links
   - `status`: DEAD, STABLE, UNSTABLE, XSTABLE, XUNSTABLE
   - `unify`, `constr`: Pending work queues
   - `cont`: And-continuation (code + Y registers)
   - `father`, `tried`, `next`, `previous`: Tree links

2. **Lifecycle**: Creation → execution → state transitions → termination
   - Created by TRY/RETRY/TRUST, CALL, etc.
   - Executed via task queue and choicehandler dispatch
   - Transition through STABLE ↔ UNSTABLE
   - Terminate as DEAD

3. **Concurrency**: Sibling and-boxes execute concurrently
   - And-parallelism: Multiple goals in parallel
   - Task queue scheduling: Cooperative multitasking
   - Fair but not guaranteed

4. **Work queues**: Defer unifications and constraints
   - Unifier queue: Pending unifications
   - Constraint queue: Pending constraints
   - Processed when agent becomes active

5. **Promotion**: Restructure tree when guard commits
   - Copy successful agent to parent level
   - Kill sibling alternatives
   - Complex copying algorithm preserves sharing

6. **Sequential vs concurrent**:
   - CALL: Sequential execution (reuse and-box)
   - TRY/RETRY/TRUST: Create sibling and-boxes
   - `&`: Explicit concurrent conjunction

And-boxes enable AGENTS to express:
- **Concurrent algorithms**: Producer-consumer, pipelines
- **Nondeterministic search**: Explore alternatives concurrently
- **Constraint propagation**: Suspended agents wake in parallel

Combined with choice-boxes (Chapter 12) and the execution model (Chapter 5), and-boxes form the core of AGENTS' concurrent constraint programming paradigm.

In the next chapters:
- **Chapter 14**: Constraint variables and suspension mechanisms
- **Chapter 15**: Port communication for agent synchronization
- **Chapter 16**: Built-in predicates and the runtime library

The and-box mechanism, together with guards and constraints, makes AGENTS uniquely powerful for concurrent, nondeterministic, constraint-based programming.

---

**Key source files:**

- `emulator/tree.h:66-93` - And-box and and-continuation structures
- `emulator/config.h:158-184` - And-box creation macros
- `emulator/storage.h:141-151` - And-box allocation
- `emulator/copy.c:1-600` - Promotion and demotion algorithms
- `emulator/engine.c:682-785` - CALL instruction
- `emulator/engine.c:3386-3565` - Choicehandler (agent scheduling)
