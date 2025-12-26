# Chapter 9: The Execution Engine

## Introduction

The execution engine is the **heart of AGENTS**—the bytecode interpreter that brings AKL programs to life. Implemented in `engine.c` (2,500+ lines), the engine executes PAM instructions, manages execution state, handles guards and suspension, and orchestrates the dance of concurrent agents.

This chapter dissects the engine's structure, explains its critical optimizations, and reveals how it implements the complex semantics of AKL.

## The `tengine()` Function

### Entry Point

Everything converges on this function:

From `engine.c:107-110`:

```c
void tengine(exstate *exs, int initcodes)
{
    /* The heart of the AKL engine! */
```

**Parameters:**
- `exs`: Execution state containing program counter, registers, and-boxes, etc.
- `initcodes`: If non-zero, initialize dispatch table and return (for threaded code setup)

**Never returns normally**: The engine runs until:
- A `halt` instruction
- An error occurs (`longjmp` to error handler)
- A `PROCEED` with no continuation (top-level return)

### Register Variables

The engine declares critical variables as `register` with explicit register hints:

From `engine.c:115-154`:

```c
register heap        *heaplimit;
register code        *pc REGISTER3;        // Program counter
register opcode      op REGISTER4;         // Current opcode
register Term        *areg REGISTER5;      // Argument registers
register Term        *yreg REGISTER6;      // Environment (Y registers)
register int         write_mode REGISTER1; // Unification mode flag
register andbox      *andb REGISTER2;      // Current and-box
register choicebox   *chb;                 // Current choice-box
register choicebox   *insert;              // Choice insertion point
```

**Why `register`?**: Hint to compiler that these variables are **hot** (accessed every instruction). Modern compilers often ignore register hints, but on platforms with HARDREGS (see Chapter 20), these map to physical CPU registers for maximum performance.

**Why numbered (`REGISTER1`-`REGISTER6`)?**: On platforms with hard register allocation, these expand to actual register names. From `regdefs.h`:

```c
#ifdef __x86_64__
#  define REGISTER1 asm("r12")
#  define REGISTER2 asm("r13")
#  define REGISTER3 asm("r14")
#  define REGISTER4 asm("r15")
#  define REGISTER5 asm("rbx")
#  define REGISTER6 asm("rbp")
#endif
```

This **keeps hot variables in CPU registers**, avoiding memory access. Huge performance win (20-40% speedup).

## Main Execution Loop

### Structure

From `engine.c:194-200`:

```c
for(;;) {    // Infinite loop

    goto choicehandler;   // Check for choice points

read_instr_dispatch:      // Main dispatch point

    SwitchTo(op) {
        CaseLabel(SWITCH_ON_TERM):
            /* Handler code */
            NextOp();

        CaseLabel(TRY):
            /* Handler code */
            NextOp();

        /* ~100+ more instruction handlers */
    }
}
```

**Key elements:**

1. **Infinite loop**: Engine never exits normally
2. **Choice handler**: Check if backtracking needed
3. **Dispatch label**: Target of instruction jumps
4. **SwitchTo macro**: Either `switch(op)` or threaded code dispatch
5. **Instruction handlers**: One per opcode
6. **NextOp macro**: Fetch next instruction and dispatch

### Dual Dispatch Mechanisms

#### Threaded Code (Fast Path)

When compiled with gcc/clang and `-DTHREADED_CODE`:

From `instructions.h:27-41`:

```c
#ifdef THREADED_CODE

#define SwitchTo(op)      /* Nothing */
#define CaseLabel(name)   CASE_##name
#define NextOp()          goto *instr_label_table[*pc++]

#else  // Switch-based fallback

#define SwitchTo(op)      switch(op) {
#define CaseLabel(name)   case name:
#define NextOp()          break; }

#endif
```

**Threaded code dispatch:**

```c
static address label_table[] = {
    &&CASE_SWITCH_ON_TERM,
    &&CASE_TRY,
    &&CASE_CALL,
    // ... ~115 entries
};

instr_label_table = label_table;

// Dispatch:
NextOp() expands to:
    goto *instr_label_table[*pc++];
```

**How it works:**
1. Read opcode from `*pc`
2. Increment pc
3. Use opcode as index into `instr_label_table`
4. Jump directly to that label's address

**Advantage**: Direct jump to handler, no branch to top of switch. Modern CPUs predict these indirect branches well (using a branch target buffer). **~20-30% faster** than switch.

**Requirement**: Compiler support for `&&label` syntax (gcc/clang extension).

#### Switch-Based Dispatch (Fallback)

When threaded code unavailable:

```c
for(;;) {
    op = *pc++;
    switch(op) {
        case SWITCH_ON_TERM:
            /* Handler code */
            break;
        case TRY:
            /* Handler code */
            break;
        /* ... */
    }
}
```

**Advantage**: Standard C, portable.

**Disadvantage**: Extra branch at end of each handler to return to top of switch.

## Instruction Handlers

### Handler Structure

Each instruction handler follows a pattern:

```c
CaseLabel(INSTRUCTION_NAME):
    /* 1. Decode operands from instruction stream */
    arg1 = pc[0];
    arg2 = pc[1];
    pc += 2;

    /* 2. Perform instruction semantics */
    result = do_something(arg1, arg2, areg, ...);

    /* 3. Check for exceptions (GC, interrupts) */
    CheckExceptions;

    /* 4. Dispatch to next instruction */
    NextOp();
```

### Example: CALL Instruction

From `engine.c:550-620` (simplified):

```c
CaseLabel(CALL):
    {
        u16 arity = *pc++;           // Read arity
        predicate *pred = *(predicate **)pc;  // Read predicate pointer
        pc += PtrSize;

        // Save continuation
        cont_pc = pc;

        // Look up predicate code
        code *target = pred->code;

        if (target == NULL) {
            // Undefined predicate
            Error("Undefined predicate");
        }

        // Jump to target code
        pc = target;
        arity = arity;   // Save arity
        def = pred;      // Save predicate

        FetchOpCode(0);  // Read next opcode
        CheckExceptions;
        NextOp();
    }
```

**What happens:**
1. Read arity and predicate from instruction stream
2. Save current pc as continuation
3. Look up predicate's code address
4. Jump to that code
5. Update pc, arity, def
6. Check for GC/interrupts
7. Dispatch

### Example: GET_X_VARIABLE

From `engine.c:700-710` (simplified):

```c
CaseLabel(GET_X_VARIABLE):
    {
        u16 x_reg = *pc++;      // Destination X register
        u16 a_reg = *pc++;      // Source argument register

        areg[x_reg] = areg[a_reg];   // Copy

        NextOp();
    }
```

Simple: copy from one register to another.

### Example: GET_STRUCTURE

From `engine.c:850-920` (simplified):

```c
CaseLabel(GET_STRUCTURE):
    {
        functor *f = *(functor **)pc;
        pc += PtrSize;
        u16 a_reg = *pc++;

        Term t = areg[a_reg];
        Deref(t);   // Follow references

        if (IsStr(t)) {
            // Match structure
            if (Functor(t) == f) {
                // Match succeeds, enter read mode
                str = AddressOf(t) + 1;   // Point to first arg
                write_mode = 0;           // Read mode
            } else {
                // Functor mismatch - backtrack
                goto backtrack;
            }
        } else if (IsUva(t)) {
            // Build structure
            str = AllocateHeap(f->arity + 1);
            *str = TagFunctor(f);
            *AddressOf(t) = TagStr(str);   // Bind variable
            str++;
            write_mode = 1;                // Write mode
        } else {
            // Type mismatch - backtrack
            goto backtrack;
        }

        NextOp();
    }
```

**Dual mode:**
- **Read mode** (matching): `t` is a structure, check functor, set `str` to point to args
- **Write mode** (building): `t` is unbound, allocate structure, bind variable, set `str` to new structure

Subsequent `UNIFY_*` instructions read from or write to `str` depending on `write_mode`.

## State Management

### Execution State Structure

From `exstate.h:417-442`:

```c
typedef struct exstate {
    struct exstate   *father;    // Parent execution state
    struct exstate   *next;      // Sibling states
    struct exstate   *child;     // Child states

    code             *pc;        // Program counter
    uword            arity;      // Current arity
    predicate        *def;       // Current predicate

    choicebox        *insert;    // Choice insertion point
    andbox           *andb;      // Current and-box
    choicebox        *root;      // Root choice-box

    trailhead        trail;      // Backtracking trail
    taskhead         task;       // Task queue
    recallhead       recall;     // Recall stack
    wakehead         wake;       // Wake queue
    contexthead      context;    // Context stack
    suspendhead      suspend;    // Suspension stack

    Term             areg[MAX_AREGS];  // Argument registers (256)

    // ... more fields
} exstate;
```

**Hierarchical structure:**
- **Father/child**: And-parallel execution tree
- **Next**: Sibling agents

**Program state:**
- **pc**: Where we are in the code
- **arity, def**: What predicate we're executing

**Control structures:**
- **andb**: Current and-box (conjunction context)
- **insert**: Where to insert choice points
- **root**: Root of choice-box tree

**Data structures:**
- **areg**: Argument registers for passing values
- **trail**: Records bindings for backtracking
- **task, recall, wake**: Queues for scheduling

### Saving and Restoring Registers

Emulator variables must be synced with execution state when calling C functions:

From `engine.c:72-84`:

```c
#define SaveRegisters \
{ \
    exs->andb = andb; \
    exs->insert = insert; \
}

#define RestoreRegisters \
{ \
    andb = exs->andb; \
    insert = exs->insert; \
    chb = andb->father; \
    yreg = andb->cont->yreg; \
}
```

**Why needed**: When we call a C function (like `unify()` or `gc()`), register variables might be lost. We must:
1. **Save** critical state to `exs` before the call
2. **Restore** from `exs` after the call

### Exception Handling

From `engine.c:87-103`:

```c
#define CheckExceptions \
{ \
    if (interruptflag) { \
        interruptflag = NO_INTRPT; \
        longjmp(toplevenv, 1); \
    } \
    if (gcflag) { \
        exs->arity = arity; \
        exs->andb = andb; \
        exs->insert = insert; \
        gc(exs); \
        andb = exs->andb; \
        insert = exs->insert; \
        chb = andb->father; \
        yreg = andb->cont->yreg; \
    } \
}
```

**Two exceptions:**

1. **Interrupt** (Ctrl-C): Set `interruptflag`, next `CheckExceptions` jumps to top level
2. **GC required**: Set `gcflag`, next `CheckExceptions` runs garbage collector

**Critical**: Check after every instruction that might allocate memory or be interrupted.

## And-Boxes and Choice-Boxes

### And-Box Structure

From `tree.h:100-130` (simplified):

```c
typedef struct andbox {
    struct choicebox *father;    // Parent choice-box
    struct andbox    *next;      // Sibling and-boxes
    struct andbox    *prev;      // Previous sibling

    continuation     *cont;      // Continuation (pc, yreg)
    struct wakenode  *wake;      // Wake list

    uword            status;     // ACTIVE, SUSPENDED, FAILED, etc.

    // ... more fields
} andbox;
```

**Represents**: A conjunction of goals (agent body).

**Relations:**
- **Father**: Parent choice-box (alternatives)
- **Next/prev**: Sibling agents (and-parallelism)

**State:**
- **cont**: Where to resume execution
- **wake**: Agents waiting for this to finish
- **status**: Is this agent running, suspended, or done?

### Choice-Box Structure

From `tree.h:150-180` (simplified):

```c
typedef struct choicebox {
    struct andbox    *current;   // Currently active and-box
    struct andbox    *children;  // List of child and-boxes

    code             *next;      // Next alternative clause
    uword            arity;      // Arity for retry

    Term             areg[MAX_AREGS];  // Saved argument registers

    trailhead        trail;      // Trail snapshot

    // ... more fields
} choicebox;
```

**Represents**: A choice point (or-parallelism).

**Contains:**
- **children**: Alternative and-boxes (different clauses)
- **next**: Code address of next clause to try
- **areg**: Saved argument registers (for backtracking)
- **trail**: Trail snapshot (for undo)

### The Execution Tree

At any moment, the engine maintains a tree:

```
                 [Root Choice-Box]
                        |
            +-----------+------------+
            |           |            |
       [And-Box 1] [And-Box 2]  [And-Box 3]
            |                        |
     [Choice-Box]              [Choice-Box]
            |                        |
      [And-Box 1.1]            [And-Box 3.1]
```

**Concurrency**: Multiple and-boxes can be active simultaneously (and-parallelism).

**Alternatives**: Choice-boxes represent backtracking points (or-parallelism).

## Guard Execution

### Guard Instructions

AKL guards are implemented with special instructions:

- **GUARD_UNIT**: Leftmost guard, commits immediately on success
- **GUARD_ORDER**: Ordered guard, must be leftmost
- **GUARD_UNORDER**: Unordered guard, can commit when solved

From `engine.c:374-487` (simplified for GUARD_UNIT):

```c
CaseLabel(GUARD_UNIT):
    {
        // Execute guard body
        // If guard succeeds, promote and-box
        // If guard fails, try next clause
        // If guard suspends, wait

        // Promote on success:
        promote_andbox(andb);

        NextOp();
    }
```

### Suspension

When a guard can't proceed (waiting for variables):

From `engine.c:500-550`:

```c
CaseLabel(SUSPEND_FLAT):
    {
        // Suspend current and-box
        andb->status = SUSPENDED;

        // Add to wake list of variable
        Term var = areg[0];
        add_to_wake_list(var, andb);

        // Switch to another agent
        schedule_next_agent();

        NextOp();
    }
```

**What happens:**
1. Mark and-box as suspended
2. Register with variable's wake list
3. Schedule another agent to run

**When variable is bound:**
1. Wake list is traversed
2. Suspended agents are reactivated
3. They resume execution

### Promotion

When a guard commits, its and-box is **promoted** to the parent context:

```c
void promote_andbox(andbox *andb) {
    // Copy and-box state to parent
    // Remove choice-boxes (no backtracking after commit)
    // Continue execution in parent context
}
```

**Effect**: The guarded body becomes part of the parent's computation. Alternative clauses are discarded (commitment).

## Backtracking

### Choice Point Creation (TRY)

From `engine.c:260-300`:

```c
CaseLabel(TRY):
    {
        code *next_clause = *(code **)pc;
        pc += PtrSize;

        // Create choice-box
        choicebox *new_chb = allocate_choicebox();
        new_chb->next = next_clause;      // Next alternative
        new_chb->arity = arity;

        // Save argument registers
        for (int i = 0; i < arity; i++) {
            new_chb->areg[i] = areg[i];
        }

        // Save trail
        new_chb->trail = trail;

        // Link into tree
        new_chb->father = chb;
        insert = new_chb;

        NextOp();
    }
```

**What's saved:**
- **next**: Address of next clause
- **areg**: Arguments (may be modified by this clause)
- **trail**: Trail pointer (for undo)

### Backtracking (RETRY)

From `engine.c:320-360`:

```c
CaseLabel(RETRY):
    {
        code *next_clause = *(code **)pc;
        pc += PtrSize;

        // Restore arguments
        for (int i = 0; i < arity; i++) {
            areg[i] = insert->areg[i];
        }

        // Undo bindings
        unwind_trail(insert->trail);

        // Update choice-box
        insert->next = next_clause;

        NextOp();
    }
```

**What happens:**
1. Restore argument registers to original values
2. Undo variable bindings (trail unwind)
3. Update next clause pointer
4. Try the alternative

### Choice Exhaustion (TRUST)

From `engine.c:370-390`:

```c
CaseLabel(TRUST):
    {
        // Last alternative
        // Restore arguments and trail
        for (int i = 0; i < arity; i++) {
            areg[i] = insert->areg[i];
        }
        unwind_trail(insert->trail);

        // Remove choice-box (no more alternatives)
        insert = insert->father;

        NextOp();
    }
```

**TRUST** means: this is the last clause, no more backtracking possible after this.

## Scheduling

AGENTS is a **sequential** implementation—one agent executes at a time. Concurrency is **simulated** by interleaving.

### Agent Selection

When an agent suspends, the engine must choose another:

```c
andbox *schedule_next_agent() {
    // 1. Check task queue
    if (task_queue_nonempty()) {
        return dequeue_task();
    }

    // 2. Check wake queue
    if (wake_queue_nonempty()) {
        return dequeue_wake();
    }

    // 3. Check siblings
    if (andb->next != NULL) {
        return andb->next;
    }

    // 4. Backtrack
    return backtrack_to_choice();
}
```

**Priority**:
1. **Tasks**: Explicit tasks queued by `call/1` etc.
2. **Woken agents**: Just unblocked by variable binding
3. **Siblings**: Other and-boxes at same level
4. **Backtracking**: Try alternative clauses

## Optimization Techniques

### 1. Register Allocation

Keep hot variables in CPU registers (see `regdefs.h`). Massive speedup.

### 2. Threaded Code Dispatch

Direct jumps instead of switch. ~20-30% faster.

### 3. Inline Instructions

Handlers are inlined in the main loop. No function call overhead.

### 4. Immediate Operands

Small constants are embedded in instruction stream. No lookup needed.

### 5. Specialized Instructions

Example: `GET_CONSTANT_X0` is optimized for getting a constant into register 0 (common case). Saves decoding overhead.

### 6. Write Mode Optimization

Unification has read/write modes. When in write mode, no checking—just allocate and write. Fast.

## Performance Characteristics

### Instruction Execution

**Typical instruction time**: 5-20 CPU cycles on modern x86-64 with threaded code.

**Breakdown**:
- 1-2 cycles: Fetch and decode
- 2-5 cycles: Execute
- 1-2 cycles: Dispatch

**Switch vs. threaded code**: Threaded code saves ~5-10 cycles per instruction (the branch back to switch).

### Critical Path

**Hottest instructions** (most executed):
1. `CALL` / `EXECUTE` - Procedure calls
2. `GET_X_VARIABLE` / `PUT_X_VALUE` - Register moves
3. `UNIFY_X_VALUE` - Unification
4. `PROCEED` - Return

These instructions determine overall performance. Optimizing them yields maximum benefit.

## Error Handling

### Failure

When an instruction fails (e.g., unification fails):

```c
goto backtrack;
```

This jumps to the backtracking handler, which:
1. Finds the most recent choice point
2. Executes RETRY or TRUST to try next clause
3. If no choice points, reports failure to user

### Errors

For actual errors (not logical failure):

```c
Error("Message");
```

Expands to:

```c
#define Error(msg) \
    do { \
        fprintf(stderr, "Error: %s\n", msg); \
        longjmp(toplevenv, 1); \
    } while (0)
```

Jumps back to top-level loop.

## Debugging Support

### Trace Instructions

When compiled with `-DTRACE`, extra instructions are inserted:

```c
CaseLabel(TRACE_CALL):
    if (tracing) {
        print_call_info(def, areg, arity);
    }
    NextOp();
```

Prints every call, return, etc. Useful for debugging but slow.

### Spy Points

Set spy points on predicates:

```
?- spy(foo/2).
```

Engine checks before each call:

```c
if (def->spypoint) {
    enter_debugger();
}
```

User can step through execution, inspect variables, etc.

## Summary

The execution engine is a marvel of low-level optimization:

- **Dual dispatch**: Threaded code or switch
- **Register allocation**: Hot variables in CPU registers
- **Inline handlers**: No function call overhead
- **And-box tree**: Concurrent agents
- **Choice-box tree**: Or-parallelism
- **Guard suspension**: Wait for constraints
- **Efficient backtracking**: Trail and choice points

Understanding the engine illuminates how AKL's high-level semantics are efficiently realized on real hardware.

The next chapters explore specific subsystems: unification, memory management, constraints, and more.

---

**Next Chapter**: [Chapter 10: Instruction Dispatch](chapter-10-instruction-dispatch.md)
