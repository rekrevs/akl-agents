# Chapter 4: The Prototype Abstract Machine (PAM)

## Introduction

The **Prototype Abstract Machine (PAM)** is the intermediate representation that bridges AKL source code and the C emulator. PAM is AGENTS' bytecode format—a set of approximately 115 instructions that the emulator interprets. Understanding PAM is essential for understanding how high-level AKL constructs are realized at a lower level.

This chapter documents the PAM instruction set, bytecode format, and the relationship to the Warren Abstract Machine (WAM) that inspired it.

## Historical Context: The Warren Abstract Machine

### WAM Origins

The Warren Abstract Machine (WAM), developed by David H.D. Warren in 1983, revolutionized Prolog implementation. Before WAM, Prolog systems were interpreters that directly executed source code—slow and inefficient. WAM introduced:

1. **Compilation to bytecode**: Prolog clauses compiled to abstract instructions
2. **Register-based architecture**: Arguments passed in registers, not on stack
3. **Specialized instructions**: Optimized for unification and control flow
4. **Efficient indexing**: First-argument indexing for clause selection

WAM became the de facto standard for logic programming implementation. Modern Prolog systems (SICStus, SWI-Prolog, YAP) are WAM-based.

### PAM: WAM Extended for AKL

PAM (Prototype Abstract Machine) extends WAM with:

**Concurrency primitives:**
- And-box creation and management
- Agent suspension and waking
- Guard instructions

**Constraint support:**
- Finite domain constraint instructions
- Constraint propagation hooks
- Suspension on constraints

**Port operations:**
- Port creation
- Asynchronous sending
- Stream manipulation

**Enhanced control flow:**
- Multiple guard types
- Promotion and commitment
- Non-determinate choice management

PAM retains WAM's core concepts (register-based, unification instructions, indexing) while adding AKL-specific features.

## Instruction Set Overview

PAM has approximately **115 instructions** organized into categories:

From `opcodes.h:1-280`, here's the complete inventory:

### Control Flow (20 instructions)

**Choice point management:**
```
SWITCH_ON_TERM        Type-based clause selection
TRY_SINGLE           Single-clause optimization (no choice point)
TRY                  Create choice point, try first clause
RETRY                Backtrack, try next clause
TRUST                Last clause (remove choice point)
```

**Procedure calls:**
```
CALL                 Call predicate with continuation
EXECUTE              Tail call (no continuation)
DEALLOC_EXECUTE      Deallocate environment and tail call
PROCEED              Return to continuation
DEALLOC_PROCEED      Deallocate and return
META_CALL            Call through variable
CALL_APPLY           Apply abstraction
EXECUTE_APPLY        Tail-apply abstraction
```

**Failure:**
```
FAIL                 Explicit failure (backtrack)
FAIL_DEC             Failure with decrement (BAM)
```

### Guard Instructions (8 instructions)

**Guard types:**
```
GUARD_UNIT           Leftmost guard (unit clause)
GUARD_ORDER          Ordered guard
GUARD_UNORDER        Unordered guard
```

**Guard control:**
```
GUARD_COND           Conditional guard
GUARD_CUT            Cut in guard
GUARD_COMMIT         Explicit commitment
GUARD_WAIT           Explicit wait (suspension)
GUARD_HALT           Halt guard execution
```

### Environment Management (2 instructions)

```
ALLOCATE             Allocate environment (Y registers)
DEALLOCATE           Deallocate environment
```

### Register Operations (30 instructions)

**GET instructions** (read into registers from arguments):
```
GET_X_VARIABLE       Get into X register
GET_Y_VARIABLE       Get into Y register
GET2_Y_VARIABLE      Get into 2 Y registers (optimization)
GET3_Y_VARIABLE      Get into 3 Y registers
GET4_Y_VARIABLE      Get into 4 Y registers
GET5_Y_VARIABLE      Get into 5 Y registers
GET_X_VALUE          Unify X register with argument
GET_Y_VALUE          Unify Y register with argument
GET_CONSTANT         Get constant
GET_NIL              Get empty list []
GET_STRUCTURE        Get structure
GET_LIST             Get list
GET_LIST_X0          Get list into X0 (optimization)
GET_NIL_X0           Get nil into X0
GET_STRUCTURE_X0     Get structure into X0
GET_CONSTANT_X0      Get constant into X0
GET_ABSTRACTION      Get abstraction (lambda)
```

**PUT instructions** (write from registers to arguments):
```
PUT_X_VOID           Put void (anonymous variable)
PUT_Y_VOID           Put void in Y register
PUT_X_VARIABLE       Put new X variable
PUT_Y_VARIABLE       Put new Y variable
PUT_X_VALUE          Put X register value
PUT_Y_VALUE          Put Y register value
PUT2_Y_VALUE         Put 2 Y values (optimization)
PUT3_Y_VALUE         Put 3 Y values
PUT4_Y_VALUE         Put 4 Y values
PUT5_Y_VALUE         Put 5 Y values
PUT_CONSTANT         Put constant
PUT_NIL              Put empty list
PUT_STRUCTURE        Put structure
PUT_LIST             Put list
PUT_ABSTRACTION      Put abstraction
```

### Unification Instructions (9 instructions)

```
UNIFY_VOID           Unify with anonymous variable
UNIFY_Y_VARIABLE     Unify with new Y variable
UNIFY_X_VARIABLE     Unify with new X variable
UNIFY_X_VALUE        Unify with X register
UNIFY_Y_VALUE        Unify with Y register
UNIFY_CONSTANT       Unify with constant
UNIFY_NIL            Unify with []
UNIFY_LIST           Unify with list cell
UNIFY_STRUCTURE      Unify with structure
```

### Indexing Instructions (2 instructions)

```
SWITCH_ON_CONSTANT   Branch on constant value
SWITCH_ON_STRUCTURE  Branch on structure functor
```

### Port Operations (1 instruction)

```
SEND3                Send message to port (3-argument form)
```

### Suspension (1 instruction)

```
SUSPEND_FLAT         Suspend on flat constraint
```

### BAM Extensions (~40 instructions)

When compiled with `-DBAM`, additional specialized instructions for optimization:

**Load instructions:**
```
LOAD_NIL             Load nil constant
LOAD_ATOM            Load atom constant
LOAD_INTEGER         Load integer constant
```

**Read instructions:**
```
READ_CAR             Read car (head of list)
READ_CDR             Read cdr (tail of list)
READ_ARG             Read structure argument
```

**Move instructions:**
```
SET_REG_REG          Move between registers
SET_REG_YVAR         Move register to Y variable
SET_YVAR_REG         Move Y variable to register
```

**Type switches:**
```
TERM_SWITCH          Multi-way type switch
ATOM_SWITCH          Switch on atom value
ATOM_TYPE            Check atom type
INTEGER_SWITCH       Switch on integer value
INTEGER_TYPE         Check integer type
LIST_SWITCH          Switch on list
STRUCT_TYPE          Check structure type
```

**Many more specialized variants** for common patterns.

BAM (Bounded Applicability Marker) instructions are compiler-generated optimizations for specific patterns. We won't detail all ~40 here, but they follow the pattern of specializing general instructions for common cases.

## Instruction Format

### Instruction Encoding

Instructions are encoded as sequences of **machine words** (uword):

```
┌──────────────────────────────────────────────────────────┐
│ Opcode (instruction number)                              │
├──────────────────────────────────────────────────────────┤
│ Operand 1 (if any)                                       │
├──────────────────────────────────────────────────────────┤
│ Operand 2 (if any)                                       │
├──────────────────────────────────────────────────────────┤
│ ...                                                      │
└──────────────────────────────────────────────────────────┘
```

**Opcode**: A word containing the instruction number (0-114+)

**Operands**: Immediate values (integers, register numbers) or pointers (to predicates, constants, etc.)

### Examples

**Simple instruction (no operands):**
```
PROCEED
```
Encoded as: `[opcode(PROCEED)]` (1 word)

**Instruction with register operand:**
```
GET_X_VARIABLE  X3, A0
```
Encoded as: `[opcode(GET_X_VARIABLE), 3, 0]` (3 words)
- Opcode
- Destination register (X3)
- Source register (A0)

**Instruction with pointer operand:**
```
CALL  foo/2
```
Encoded as: `[opcode(CALL), arity, ptr_to_predicate]` (3 words on 64-bit)
- Opcode
- Arity (2)
- Pointer to predicate structure (2 words on 64-bit)

### Variable-Length Instructions

Instructions have variable length depending on operands:
- **0 operands**: 1 word (PROCEED, FAIL)
- **1 operand**: 2 words (GET_X_VARIABLE with register number)
- **2 operands**: 3 words (CALL with arity and predicate)
- **Pointer operands**: Extra words for 64-bit pointers

The program counter (`pc`) steps through the instruction stream, reading operands as needed.

## Key Instruction Details

### CALL and EXECUTE

**CALL** performs a non-tail call:

```
CALL arity, predicate
```

**Semantics:**
1. Save current pc as continuation
2. Jump to predicate's code
3. On PROCEED, return to saved pc

**EXECUTE** performs a tail call:

```
EXECUTE arity, predicate
```

**Semantics:**
1. No continuation saved (tail position)
2. Jump to predicate's code
3. On PROCEED, return to caller's continuation

**Optimization**: EXECUTE reuses the current stack frame, avoiding allocation.

### GET and PUT Instructions

These implement Warren's **argument passing convention**:

**GET instructions** appear in clause heads:
```
% foo(X, Y) :- ...
GET_X_VARIABLE  X0, A0    % X = A0
GET_X_VARIABLE  X1, A1    % Y = A1
```

**PUT instructions** appear before calls:
```
% ... bar(X, Y) ...
PUT_X_VALUE  A0, X0       % A0 = X
PUT_X_VALUE  A1, X1       % A1 = Y
CALL bar/2
```

**Argument registers** (`A0`, `A1`, ...) are actually the first elements of the `areg` array. The distinction between "X registers" and "argument registers" is conceptual—they're the same physical registers.

### UNIFY Instructions

UNIFY instructions operate in **read or write mode** depending on context:

```
GET_STRUCTURE  point/2, A0
UNIFY_X_VALUE  X0
UNIFY_X_VALUE  X1
```

**Read mode** (A0 is a structure):
- UNIFY_X_VALUE unifies X0 with first argument
- UNIFY_X_VALUE unifies X1 with second argument

**Write mode** (A0 is unbound):
- UNIFY_X_VALUE writes X0 as first argument
- UNIFY_X_VALUE writes X1 as second argument

The mode is set by GET_STRUCTURE/GET_LIST and stored in `write_mode` register.

### TRY, RETRY, TRUST

These implement **choice points** for multiple clauses:

```
% foo(1).
% foo(2).
% foo(3).

% Compiled to:
foo/1:
    SWITCH_ON_TERM  ...
    TRY  clause2
clause1:
    GET_CONSTANT  1, A0
    PROCEED
clause2:
    RETRY  clause3
    GET_CONSTANT  2, A0
    PROCEED
clause3:
    TRUST
    GET_CONSTANT  3, A0
    PROCEED
```

**TRY**: Create choice point, save registers, note next clause
**RETRY**: Restore registers, undo trail, try this clause
**TRUST**: Last clause, remove choice point

### GUARD Instructions

**GUARD_UNIT** implements unit clauses (leftmost guard):

```
% foo(X) :- X > 0 ? bar(X).

foo/1:
    GET_X_VARIABLE  X0, A0
    GUARD_UNIT
    % Guard code: X > 0
    % If succeeds, promote and continue
    % If fails, try next clause
    PUT_X_VALUE  A0, X0
    CALL  bar/1
    PROCEED
```

**GUARD_ORDER** and **GUARD_UNORDER** handle ordered/unordered guards with specific suspension and commitment semantics.

### SEND3

Port communication:

```
SEND3  message, port, continuation
```

**Semantics:**
1. Extend port stream with message
2. Non-blocking (always succeeds)
3. Continue with continuation

This is the primitive for asynchronous message passing.

## Bytecode File Format (.pam files)

### File Structure

A `.pam` file contains compiled predicates:

```
┌────────────────────────────────────────┐
│ Header                                 │
│  - Magic number                        │
│  - Version                             │
│  - Platform info                       │
├────────────────────────────────────────┤
│ Atom Table                             │
│  - List of atom names                  │
├────────────────────────────────────────┤
│ Functor Table                          │
│  - List of functor names and arities   │
├────────────────────────────────────────┤
│ Predicate Definitions                  │
│  - For each predicate:                 │
│    * Name and arity                    │
│    * Code size                         │
│    * Bytecode instructions             │
│    * Indexing info                     │
├────────────────────────────────────────┤
│ Relocation Information                 │
│  - References to be resolved at load   │
└────────────────────────────────────────┘
```

### Loading Process

From `load.c`:

1. **Read header**: Verify magic number, version compatibility
2. **Load atom table**: Create/lookup atoms in global table
3. **Load functor table**: Create/lookup functors
4. **Load predicates**:
   - Allocate space for code
   - Read bytecode
   - Create predicate structure
   - Link to atom/functor tables
5. **Resolve relocations**: Fix up references
6. **Install predicates**: Add to global predicate table

After loading, predicates are callable.

### Portability

PAM files are **mostly portable** but have platform dependencies:

**Portable:**
- Instruction opcodes
- Register numbers
- Atom/functor names

**Platform-dependent:**
- Word size (32 vs 64-bit)
- Pointer size
- Endianness (in principle, though AGENTS assumes little-endian)

In practice, PAM files compiled on one 64-bit platform work on another 64-bit platform (x86-64 ↔ ARM64).

## Relationship to AKL Source

### Example Compilation

**AKL source:**
```
append([], Ys, Ys).
append([X|Xs], Ys, [X|Zs]) := append(Xs, Ys, Zs).
```

**PAM bytecode (simplified):**
```
append/3:
    SWITCH_ON_TERM  list_case, var_case, fail_case
    TRY  clause2

clause1:  % append([], Ys, Ys).
    GET_NIL  A0                  % Match [] in A0
    GET_X_VARIABLE  X0, A1       % X0 = A1 (Ys)
    GET_X_VALUE  X0, A2          % Unify X0 with A2 (Ys = Ys)
    PROCEED

clause2:  % append([X|Xs], Ys, [X|Zs]) :- append(Xs, Ys, Zs).
    TRUST
    GET_LIST  A0                 % Match [_|_] in A0
    UNIFY_X_VARIABLE  X0         % X0 = X (head)
    UNIFY_X_VARIABLE  X1         % X1 = Xs (tail)
    GET_X_VARIABLE  X2, A1       % X2 = Ys
    GET_LIST  A2                 % Match [_|_] in A2
    UNIFY_X_VALUE  X0            % Unify with X
    UNIFY_X_VARIABLE  X3         % X3 = Zs

    % Recursive call: append(Xs, Ys, Zs)
    PUT_X_VALUE  A0, X1          % A0 = Xs
    PUT_X_VALUE  A1, X2          % A1 = Ys
    PUT_X_VALUE  A2, X3          % A2 = Zs
    EXECUTE  append/3            % Tail call
```

### Compilation Strategy

The compiler performs:

1. **Clause head analysis**: Determine argument patterns
2. **Register allocation**: Assign X/Y registers
3. **Instruction selection**: Choose appropriate GET/PUT/UNIFY
4. **Optimization**: Use specialized instructions (GET_NIL_X0, etc.)
5. **Indexing**: Generate SWITCH_ON_TERM for first-argument indexing
6. **Choice point insertion**: Add TRY/RETRY/TRUST

See Chapter 17 for compiler details.

## Instruction Set Philosophy

### Why These Instructions?

PAM's instruction set reflects **implementation pragmatics**:

**Abstraction level**: High enough to be platform-independent, low enough to map efficiently to C.

**Specialization**: Common patterns get specialized instructions (GET_NIL_X0, PUT2_Y_VALUE) for efficiency.

**Type-specific operations**: Separate instructions for lists vs structures vs atoms, enabling optimized handlers.

**Unification granularity**: UNIFY instructions operate on single arguments, allowing interleaving with other operations.

**Choice point explicitness**: TRY/RETRY/TRUST make backtracking structure clear, enabling optimization.

**Guard primitives**: First-class guard instructions enable efficient implementation of AKL's key feature.

### Comparison with Other Abstract Machines

**vs. WAM:**
- PAM adds concurrency (and-boxes)
- PAM adds guards (suspension, commitment)
- PAM adds ports (SEND3)
- PAM adds constraints (SUSPEND_FLAT, FD instructions)
- PAM is otherwise similar (register-based, unification, indexing)

**vs. BEAM (Erlang):**
- BEAM focuses on process isolation and fault tolerance
- PAM focuses on shared constraint stores and logic
- BEAM has no unification (pattern matching only)
- PAM has no process mailboxes (ports serve similar role)

**vs. JVM:**
- JVM is stack-based, PAM is register-based
- JVM has objects and methods, PAM has terms and predicates
- JVM is typed, PAM is dynamically typed
- Both compile high-level languages to bytecode

**vs. Python bytecode:**
- Python bytecode is stack-based, PAM is register-based
- Python is imperative, PAM is declarative
- Similar in being interpreted VMs

PAM is unusual in combining logic programming (unification, backtracking) with concurrency (and-boxes) and constraints (suspension).

## Performance Implications

### Instruction Density

PAM instructions are **relatively high-level**:
- One CALL does a lot (save continuation, jump, set up state)
- One GET_STRUCTURE does unification, mode setting, dereferencing

**Advantage**: Fewer instructions executed per operation
**Disadvantage**: Each instruction does more work (longer handlers)

### Threaded Code

Threaded code dispatch (Chapter 10) is crucial for PAM performance:
- Direct jumps avoid switch overhead
- Well-predicted by modern CPUs
- ~20-30% speedup vs. switch

### Register Pressure

PAM assumes many registers are available:
- areg[0..255]: Argument registers
- X registers: Temporary values
- Y registers: Environment variables
- Emulator variables: pc, op, andb, etc.

On platforms with hard register allocation, keeping hot values in CPU registers is a huge win.

### Instruction Expansion

Some PAM instructions expand to substantial C code:
- GET_STRUCTURE: 30-50 lines of C
- TRY: Create choice-box, save state, link structures
- GUARD_UNIT: Complex control flow with suspension/promotion

The amortization is in avoiding function calls and enabling inlining.

## Future Extensions

Potential PAM enhancements:

**1. JIT compilation**: Translate hot bytecode sequences to native code
**2. Parallel instructions**: Explicit parallel and-box creation
**3. Optimized constraints**: Inline constraint propagation
**4. Native integration**: Better foreign function interface
**5. Profiling support**: Built-in instrumentation

The bytecode format is extensible—new opcodes can be added without breaking existing code (as long as old opcodes remain compatible).

## Summary

PAM is AGENTS' intermediate representation:

- **~115 instructions** extending WAM for AKL
- **Register-based** with argument passing convention
- **Variable-length encoding** in machine words
- **Specialized instructions** for common patterns
- **Guard primitives** for synchronization and commitment
- **Port operations** for asynchronous communication
- **Bytecode files** (.pam) are mostly portable
- **Threaded code dispatch** for performance

Understanding PAM is essential for understanding how AKL constructs (guards, ports, constraints) are implemented. The instruction set reveals the pragmatic engineering decisions that make AGENTS efficient.

The next chapter explores the execution model: how and-boxes, choice-boxes, and agents interact during execution.

---

**Next Chapter**: [Chapter 5: Execution Model](chapter-05-execution-model.md)
