# Chapter 14: Constraint Variables and Suspension

## Introduction

One of AGENTS' most powerful features is its ability to **suspend** execution when constraints are not yet satisfied, and **wake** suspended agents when variables become bound. This enables:

1. **Concurrent constraint programming**: Agents coordinate through shared constraint variables
2. **Lazy evaluation**: Computation waits for needed data
3. **Synchronization**: Guards suspend until conditions are met
4. **Constraint propagation**: Changes wake affected agents

This chapter explores:

- **Constraint variables** (SVA and CVA)
- **Suspension mechanism** and wake queues
- **SUSPEND_FLAT instruction**
- **Variable binding and waking**
- **Finite domain constraints** (overview)

The suspension mechanism transforms AGENTS from a simple logic programming system into a **concurrent constraint system** where agents can wait for information before proceeding.

## Variable Types

AGENTS distinguishes several types of variables based on their constraint status.

### UVA: Unbound Unconstrained Variable

**Definition**: A simple unbound variable with no constraints.

**Tag**: `TAG_UVA` (value 1)

**Structure**: Just a tagged pointer to an environment:

```c
// UVA is represented as:
RefTerm(V) = TagUva(&andb->env);
```

No additional structure needed—the variable simply points to its home environment.

### SVA: Suspended Variable with Attributes

**Definition**: An unbound variable with **suspensions**—agents waiting for it to be bound.

**Tag**: `TAG_GVA` (value 3), but with `method == NULL`

**Structure** (from `emulator/term.h:665-669`):

```c
typedef struct svainfo {
  struct gvamethod *method;     // Always NULL for SVA
  envid            *env;         // Home environment
  struct suspension *susp;       // Suspension list
} svainfo;
```

**Key insight**: SVA is a **GVA** (Global Variable with Attributes) where `method == NULL`.

**Check macro**:

```c
#define GvaIsSva(x)  ((x)->method == NULL)
```

### CVA: Constrained Variable

**Definition**: An unbound variable with **constraints** (not just suspensions).

**Tag**: `TAG_GVA`, with `method != NULL`

**Structure**: Same as SVA, but `method` points to a constraint method table:

```c
typedef struct svainfo {  // Same structure, different interpretation
  struct gvamethod *method;     // Non-NULL for CVA
  envid            *env;
  struct suspension *susp;
} svainfo;

typedef struct gvamethod {
  bool (*unify)();       // Custom unification
  void (*wake)();        // Wake suspensions
  // ... other constraint methods
} gvamethod;
```

**Examples**:
- **Finite domain**: `X in 1..10` creates CVA with FD constraint
- **Rational trees**: Cyclic terms use CVA

### Summary Table

| Type | Tag | Structure | Meaning |
|------|-----|-----------|---------|
| **UVA** | TAG_UVA (1) | Just env pointer | Simple unbound variable |
| **SVA** | TAG_GVA (3) | svainfo, method=NULL | Unbound with suspensions |
| **CVA** | TAG_GVA (3) | svainfo, method≠NULL | Unbound with constraints |
| **Bound** | Any | N/A | Variable bound to value |

## Suspension Structure

### Suspension List

When an agent needs to wait for a variable, it creates a **suspension** (from `emulator/tree.h:152-156`):

```c
typedef enum {
  CHB,       // Suspend a choice-box
  ANDB       // Suspend an and-box
} susptag;

typedef union redo {
  andbox    *andb;
  choicebox *chb;
} redo;

typedef struct suspension {
  susptag           type;       // What kind of suspension
  struct suspension *next;      // Next in list
  redo              suspended;  // The suspended box
} suspension;
```

**Purpose**: Record which agent is waiting on a variable.

**List structure**: Suspensions form a linked list:

```
SVA/CVA → suspension₁ → suspension₂ → suspension₃ → NULL
           (andb₁)      (andb₂)      (chb₁)
```

When the variable is bound, all suspended agents are **woken**.

### Creating a Suspension

**From initialization** (term.h:484-489):

```c
#define InitSvainfo(S, Ab) \
{ \
  (S)->method = NULL;                    // SVA, not CVA \
  (S)->env = (envid*)&((Ab)->env);       // Home environment \
  (S)->susp = NULL;                      // No suspensions yet \
}
```

**Adding a suspension**:

```c
// Allocate suspension
suspension *susp;
NewSuspension(susp);

// Initialize
susp->type = ANDB;
susp->suspended.andb = andb;    // This agent suspends

// Add to variable's suspension list
susp->next = sva->susp;
sva->susp = susp;
```

Now when the variable binds, `andb` will be woken.

## The SUSPEND_FLAT Instruction

The **SUSPEND_FLAT** instruction implements suspension at the bytecode level.

### Instruction Format

**From opcodes.h**:

```c
Item(SUSPEND_FLAT)
```

**Arguments** (from parser.y:945-952):

```
SUSPEND_FLAT(index, label, size)
```

- **index**: Register containing the variable to check
- **label**: Where to jump if variable is bound
- **size**: Arity (for continuation)

### Semantics

**Pseudo-code**:

```c
CaseLabel(SUSPEND_FLAT):
  indx i;
  code *label;
  int size;

  Instr_Index_Label_Size(SUSPEND_FLAT, i, label, size);

  Term X = areg[i];
  Deref(X, X);

  if(IsVar(X)) {
    // Variable is unbound → SUSPEND

    // Upgrade to SVA if needed
    if(VarIsUVA(Ref(X))) {
      MakeBigVar(Ref(X));    // UVA → SVA
    }

    // Create suspension for this agent
    suspension *susp;
    NewSuspension(susp);
    susp->type = ANDB;
    susp->suspended.andb = andb;

    // Add to variable's suspension list
    svainfo *sva = RefGva(Ref(X));
    susp->next = sva->susp;
    sva->susp = susp;

    // Agent suspends (goes to choicehandler)
    goto choicehandler;

  } else {
    // Variable is bound → CONTINUE
    pc = label;
    FetchOpCode(0);
    NextOp();
  }
```

**Effect:**

- If variable **unbound**: Suspend agent, add to suspension list
- If variable **bound**: Continue execution at `label`

### Example AKL Code

```prolog
p(X) :=
  suspend_on(X),    % Wait until X is bound
  process(X).       % Then process it

q :- X = 42 & p(X).  % p suspends, then wakes when X=42
```

Compiles to:

```
p/1:
  GET_X_VARIABLE X0 X1        ; X = arg1
  SUSPEND_FLAT X1, L1, 1      ; Suspend if X unbound
L1:
  CALL process/1              ; X is bound, proceed
  PROCEED

q/0:
  PUT_CONSTANT 42, X1         ; X = 42
  PUT_X_VALUE X1, X0
  CALL p/1                    ; p may suspend
  PROCEED
```

## Variable Binding and Waking

### Binding with Wake

When a variable is bound, suspended agents must be woken.

**Macro** (from exstate.h:56-76):

```c
#define BindVariable(W, A, V, T, BindFail) \
{ \
  if(VarIsUVA(V)) { \
    // Unbound unconstrained variable \
    if(IsLocalUVA(V, A)) { \
      BindCheck(V, T, BindFail);      // Just bind \
    } else if (!bind_external_uva(W, A, V, T)) { \
      BindFail; \
    } \
  } else { \
    // GVA (SVA or CVA) \
    if(GvaIsSva(RefGva(V))) { \
      // SVA: has suspensions \
      if(IsLocalGVA(V, A)) { \
        WakeAll(W, V);                // Wake suspensions \
        BindCheck(V, T, BindFail);    // Then bind \
      } else if (!bind_external_sva(W, A, V, T)) { \
        BindFail; \
      } \
    } else { \
      // CVA: has constraints \
      if (!(RefGva(V)->method->unify(TagRef(V), T, A, W))) { \
        BindFail;                     // Constraint may reject \
      } \
    } \
  } \
}
```

**Key steps:**

1. **Check variable type** (UVA, SVA, CVA)
2. **Check locality** (local vs external to current and-box)
3. **Wake suspensions** (for SVA)
4. **Check constraints** (for CVA)
5. **Bind** the variable

### WakeAll Macro

**Conceptual definition**:

```c
#define WakeAll(W, V) \
{ \
  svainfo *sva = RefGva(V); \
  suspension *susp = sva->susp; \
  \
  while(susp != NULL) { \
    if(susp->type == ANDB) { \
      // Wake and-box \
      Wake(W, susp->suspended.andb); \
    } else { \
      // Wake choice-box \
      Recall(W, susp->suspended.chb); \
    } \
    susp = susp->next; \
  } \
  \
  sva->susp = NULL;    // Clear suspension list \
}
```

**Wake operation**:

```c
#define Wake(W, Andb) \
{ \
  // Add to wake queue \
  (W)->wake.current->andb = (Andb); \
  (W)->wake.current++; \
}
```

The agent is added to the **wake queue**, which is processed by the choicehandler.

### Wake Queue Processing

**From choicehandler** (engine.c:3386-3565, simplified):

```c
choicehandler:
{
  task *entry;

  // Process task queue first
  while(!EmptyTask(exs)) {
    NextTask(exs, entry);
    // ... dispatch agents
  }

  // Process wake queue
  while(!EmptyWake(exs)) {
    andbox *wand;
    NextWake(exs, wand);      // Pop from wake queue

    if(Live(wand)) {
      // Agent is still alive, reactivate it
      andb = wand;
      chb = andb->father;
      pc = andb->cont->label;
      FetchOpCode(0);
      yreg = andb->cont->yreg;
      NextOp();               // Execute woken agent
    }
  }

  // ... try alternatives, etc.
}
```

**Flow:**

1. Variable binds → `WakeAll` adds agents to wake queue
2. Choicehandler processes wake queue
3. Each woken agent resumes execution
4. Agent retries the suspended instruction (now variable is bound)

## Example: Producer-Consumer

### AKL Code

```prolog
% Producer generates stream
producer(S) :=
  S = [1|S1],
  producer(S1).

% Consumer processes stream
consumer([X|Xs]) :=
  process(X),
  consumer(Xs).

% Run both concurrently
run :- producer(S) & consumer(S).
```

### Execution Trace

1. **Start**: `run` calls `producer(S) & consumer(S)`
   - Creates sibling and-boxes for producer and consumer
   - `S` is unbound UVA

2. **Producer executes**: `S = [1|S1]`
   - Binds `S` to `[1|S1]`
   - No suspensions yet (consumer hasn't accessed `S`)

3. **Consumer executes**: Match `[X|Xs]`
   - Tries to unify `S` with `[X|Xs]`
   - `S` is bound, unification succeeds
   - `X = 1`, `Xs = S1`

4. **Consumer processes**: `process(1)`
   - Executes process
   - Recursive call: `consumer(S1)`

5. **Consumer recurses**: `consumer(S1)` tries to match `[X|Xs]`
   - `S1` is unbound!
   - **SUSPEND_FLAT** instruction suspends consumer
   - Adds suspension to `S1`

6. **Producer continues**: Eventually binds `S1 = [2|S2]`
   - **WakeAll** wakes consumer
   - Consumer added to wake queue

7. **Choicehandler**: Processes wake queue
   - Reactivates consumer
   - Consumer retries match on `S1` (now bound to `[2|S2]`)
   - Unification succeeds, continues

8. **Repeat**: Producer and consumer alternate, synchronized via suspension/wake

**Key insight**: Suspension allows consumer to **wait** for producer without busy-waiting or polling.

## Finite Domain Constraints

Finite domain (FD) constraints are implemented as **CVA** variables with special methods.

### FD Variable

**Example AKL**:

```prolog
X in 1..10,      % X is constrained to domain {1,2,...,10}
Y in 5..15,      % Y is constrained to domain {5,6,...,15}
X #< Y,          % Constraint: X < Y
X + Y #= 20.     % Constraint: X + Y = 20
```

### FD Constraint Structure

**Conceptual**:

```c
typedef struct fd_constraint {
  int min;       // Minimum value
  int max;       // Maximum value
  // ... more fields for propagation
} fd_constraint;

typedef struct fd_method {
  bool (*unify)();       // Check if value in domain
  void (*wake)();        // Propagate constraints
  void (*narrow)();      // Reduce domain
  // ...
} fd_method;
```

When `X in 1..10` executes:

1. **Create CVA**: Upgrade `X` from UVA to CVA
2. **Attach constraint**: `X->method = &fd_method`
3. **Store domain**: `X->constraint_data = {min:1, max:10}`

### Constraint Propagation

When a constraint variable is bound or narrowed:

1. **Wake suspensions**: Other constraints waiting on this variable
2. **Propagate**: Each woken constraint re-evaluates
3. **Narrow domains**: Constraints may further restrict variables
4. **Cascade**: Changes may wake more constraints

**Example**:

```prolog
X in 1..10, Y in 1..10, X + Y #= 15, X = 8.
```

**Execution**:

1. `X in 1..10`: X becomes CVA with domain [1,10]
2. `Y in 1..10`: Y becomes CVA with domain [1,10]
3. `X + Y #= 15`: Constraint suspends on X and Y
4. `X = 8`:
   - Check: 8 ∈ [1,10]? Yes
   - Bind X = 8
   - Wake constraint `X + Y #= 15`
5. **Propagate**: `X + Y #= 15` and `X = 8` → `Y = 7`
   - Check: 7 ∈ [1,10]? Yes
   - Bind Y = 7
6. **Success**: Solution found: X=8, Y=7

### FD Implementation

The FD constraint solver is in `emulator/fd.c` (~2000 lines). Key operations:

- **domain_narrowing**: Reduce domain based on constraints
- **arc_consistency**: Ensure each value has supporting values in other variables
- **suspension**: Wait for variable bindings
- **propagation**: Wake and re-evaluate constraints

## Performance Considerations

### Suspension Overhead

**Cost of suspension**:

1. **Upgrade UVA → SVA**: Allocate svainfo (~24 bytes)
2. **Create suspension**: Allocate suspension struct (~24 bytes)
3. **Link into list**: Pointer updates
4. **Context switch**: Agent yields to choicehandler

**Total**: ~50-100 cycles

**Amortization**: If variable binds after many operations, suspension is cheaper than busy-waiting.

### Wake Queue

**Wake queue is a stack**:

```c
typedef struct wakehead {
  int   size;
  wake  *start;
  wake  *current;    // Stack pointer
  wake  *end;
} wakehead;
```

**Processing**: LIFO (last woken, first executed)

**Fairness**: Not guaranteed—depends on wake order

### Memory

**Per suspended variable**:

- **svainfo**: 24 bytes (64-bit: 3 pointers)
- **suspension**: 24 bytes per suspended agent

**Garbage collection**: Suspensions are reachable from variables, collected when variable is bound or dead.

## Advanced Topics

### External Variables

Variables can be **external** to an and-box (belong to a different agent).

**Binding external variables** requires:

1. **Trail**: Record binding for backtracking
2. **Wake**: Add suspensions to wake queue
3. **Check**: Ensure binding doesn't violate constraints

**Function**: `bind_external_sva(W, A, V, T)`

### Rational Trees

**Cyclic terms** use CVA with special unification:

```prolog
X = f(X).    % Infinite term: f(f(f(...)))
```

**Implementation**:

1. **Detect cycle**: During unification
2. **Create CVA**: With rational tree constraint
3. **Unify**: Using occurs check or rational unification

### Port Communication

**Ports** use suspension for synchronization:

```prolog
send(Port, Message).     % Suspend if port full
receive(Port, Message).  % Suspend if port empty
```

**Implementation**: Port has suspension lists for waiting senders and receivers.

## Summary

Constraint variables and suspension enable AGENTS' concurrent constraint programming:

1. **Variable types**:
   - **UVA**: Simple unbound variable
   - **SVA**: Unbound with suspensions (method=NULL)
   - **CVA**: Unbound with constraints (method≠NULL)

2. **Suspension mechanism**:
   - **SUSPEND_FLAT** instruction checks variable, suspends if unbound
   - **Suspension list** records waiting agents
   - **Wake queue** reactivates suspended agents when variable binds

3. **Variable binding**:
   - **BindVariable** macro handles UVA/SVA/CVA
   - **WakeAll** adds suspensions to wake queue
   - **Choicehandler** processes wake queue

4. **Finite domain constraints**:
   - **CVA** with FD method table
   - **Constraint propagation** narrows domains
   - **Suspension** waits for bindings

5. **Applications**:
   - **Producer-consumer**: Stream communication
   - **Constraint solving**: CSP problems
   - **Synchronization**: Guards and ports

The suspension mechanism transforms AGENTS from sequential logic programming into **concurrent constraint programming**, enabling:

- **Lazy evaluation**: Wait for needed data
- **Synchronization**: Coordinate concurrent agents
- **Constraint propagation**: Solve complex CSPs

Combined with:
- **And-parallelism** (Chapter 13)
- **Choice points** (Chapter 12)
- **Guards** (Chapter 5)

Suspension makes AGENTS uniquely powerful for concurrent, constrained, synchronized programming.

---

**Key source files:**

- `emulator/term.h:665-669` - svainfo structure
- `emulator/tree.h:152-156` - suspension structure
- `emulator/exstate.h:56-76` - BindVariable macro
- `emulator/engine.c` - SUSPEND_FLAT instruction, wake queue
- `emulator/fd.c` - Finite domain constraint solver
