# Chapter 17: The Compiler

## Introduction

The AGENTS system includes a **self-hosting compiler** that translates AKL source code (`.akl` files) into PAM bytecode (`.pam` files). Remarkably, this compiler is itself written entirely in AKL—approximately **3,500 lines** of pure constraint logic code distributed across 20 modules in the `oldcompiler/` directory.

The compiler demonstrates the expressiveness of the AKL language: a complex program performing sophisticated analysis, optimization, and code generation, written using the very language features it compiles. This chapter examines the compiler's architecture, compilation pipeline, code generation strategies, and the bootstrapping process that makes the system self-sufficient.

## Historical Context

The `oldcompiler/` directory contains the **operational compiler** used in production. The `compiler/` directory contains a later, experimental redesign that was never completed. All references in this chapter are to the working `oldcompiler/` implementation.

From `oldcompiler/README`:

> This is a compiler for the general deep guard scheme. It replaces the previous version, formerly a Prolog program. This version is a pure AKL program. Allocation of y-registers has been improved. Allocation of x-registers is still very naive.

The evolution from Prolog to AKL implementation allowed the compiler to leverage:
- **Concurrent compilation** of independent clauses
- **Constraint-based analysis** for register allocation
- **Pattern matching** for code transformations
- **Ports** for stream-based I/O

## Compiler Architecture

The compiler follows a multi-stage pipeline, each stage transforming the representation toward executable bytecode:

```
.akl source
    ↓
[Reader] → Term structures
    ↓
[Parser] → Parsed clauses (head/guard/body separation)
    ↓
[Analyzer] → Variable occurrence info
    ↓
[Try/Retry/Trust Generator] → Indexing code
    ↓
[Head Compiler] → get_* instructions
    ↓
[Body Compiler] → put_* and call instructions
    ↓
[Register Allocator] → X/Y register assignments
    ↓
[Instruction Generator] → Concrete PAM instructions
    ↓
[Peephole Optimizer] → Optimized instruction stream
    ↓
[Writer] → .pam bytecode
```

### Module Organization

The `oldcompiler/Makefile` lists the key modules:

```make
SRC = allocate.akl \
      analyze.akl \
      aux.akl \
      body.akl \
      builtin.akl \
      clause.akl \
      compiler.akl \
      edit.akl \
      fd_akl.akl \
      fd_arith_ask.akl \
      fd_arith_compile.akl \
      fd_arith_tell.akl \
      fd_mon.akl \
      fd_range.akl \
      graph.akl \
      head.akl \
      instr.akl \
      main.akl \
      modes.akl \
      parser.akl \
      reader.akl \
      try.akl \
      unify.akl \
      writepam.akl
```

These modules are compiled to `.pam` files and concatenated into `comp.pam` (~326KB), which is then loaded by the emulator to provide the `compilef/1` and `compile/1` predicates.

## Entry Points

### Main Interface (`main.akl`)

The compiler exposes two primary entry points:

**1. `compilef(Base)` - Compile to file:**

```prolog
compilef(Base) :=
    compilef(Base, []).

compilef(Base, ModeList) :=
    ->  bases_to_akl_pam(Base, Base, InFile, OutFile),
        compile_file_to_file(InFile, OutFile, ModeList).
```

Usage:
```prolog
?- compilef(myprogram).
{compiling myprogram.akl...}
{myprogram.akl compiled, 1247 ms}
```

This reads `myprogram.akl` and writes `myprogram.pam`.

**2. `compile(Base)` - Compile and load:**

```prolog
compile(Base, ModeList) :=
    ->  unix(mktemp('/tmp/aklXXXXXX', Temp)),
        bases_to_akl_pam(Base, Temp, InFile, OutFile),
        compile_file_to_file(InFile, OutFile, ModeList)
        &
        load_quietly(OutFile)
        &
        atom_to_chars(OutFile, CharsFile),
        '$concat'("/bin/rm ", CharsFile, CharsCommand),
        chars_to_atom(CharsCommand, Command),
        unix(shell(Command)).
```

This compiles to a temporary file, loads it concurrently, and cleans up—all in parallel using AKL's concurrency operators.

### Compilation Modes

The `translate_modelist/2` predicate in `modes.akl` supports compilation modes:

- **Deep guard mode** — Full guard execution with backtracking
- **Flat guard mode** — Optimized single-clause guards

The distinction affects code generation for guard operators (`->`, `?`, `|`, `!`).

## Stage 1: Reading and Parsing

### Reader (`reader.akl`)

The reader loads AKL source files and parses them into term structures using the emulator's built-in parser. It handles:
- Multi-clause definitions
- Compiler directives
- Syntax errors

### Parser (`parser.akl:1-59`)

The parser normalizes clauses into a canonical form separating head, guard, operator, and body:

```prolog
parse_clause(Clause, ParsedClause) :=
    normalise_clause(Clause, NormalClause),
    parse_normal_clause(NormalClause, ParsedClause).

normalise_clause(struct((:-),2,[Head,Body]), NormalClause) :-
    ->  normalise_body(Body, NormalBody),
        NormalClause = clause(Head, NormalBody).

normalise_clause(Head, NormalClause) :=
    ->  normalise_body(struct(true,0,[]), NormalBody),
        NormalClause = clause(Head, NormalBody).
```

**Guard Operator Recognition:**

```prolog
parse_guard_operator((->), Op) :- -> Op = cond.      % Conditional
parse_guard_operator((|), Op) :-  -> Op = commit.    % Commit
parse_guard_operator((?), Op) :-  -> Op = wait.      % Wait
parse_guard_operator((!), Op) :-  -> Op = cut.       % Cut
```

**Parsed Clause Structure:**

```prolog
ParsedClause = clause(
    Head,          % struct(functor, arity, args)
    Guard,         % list of goals
    Op,            % cond | commit | wait | cut
    Body           % list of goals
)
```

**Goal Flattening:**

Conjunctions are flattened into goal lists:

```prolog
parse_goal(struct(',',2,[P,Q]), S0, S) :-
    ->  parse_goal(P, S0, S1),
        parse_goal(Q, S1, S).
parse_goal(struct(true,0,[]), S0, S) :-
    ->  S = S0.
parse_goal(X, S0, S) :=
    ->  S0 = [X|S].
```

Example:
```prolog
% Input:
foo(X,Y) := a(X), b(Y), c(X,Y).

% Parsed:
clause(
    struct(foo, 2, [var('X'), var('Y')]),
    [],  % empty guard
    wait,
    [struct(a,1,[var('X')]),
     struct(b,1,[var('Y')]),
     struct(c,2,[var('X'), var('Y')])]
)
```

## Stage 2: Variable Analysis

### Occurrence Analysis (`analyze.akl:0-63`)

The analyzer determines how each variable is used to guide register allocation. It tracks:

- **void** — Variable never used (optimized away)
- **first** — First occurrence (allocation site)
- **middle** — Intermediate occurrence
- **last** — Final occurrence (can deallocate)

**Data Structures:**

```prolog
% Variable info: vi(GoalNumber, Occurrence, Register)
vi(3, first, x(2))    % First use of a variable in goal 3, allocated to X2
vi(5, middle, x(2))   % Middle use in goal 5, still in X2
vi(7, last, x(2))     % Last use in goal 7, X2 can be reused

% Register info: ri(Occurrence, Register)
ri(void, none)        % Unused variable
ri(first, x(5))       % First occurrence in X5
ri(last, y(2))        % Last occurrence in Y2
```

**Analysis Algorithm** (`analyze.akl:15-48`):

```prolog
analyze_group([V]) :=
    ->  V = vi(_,void,none).   % Single occurrence → void

analyze_group(VI0) :=
    ->  VI0 = [vi(N,first,R)|VI],
        analyze_multiple(VI, N, R).

analyze_multiple([V], N, R) :=
    V = vi(N,_,_)
    ->  R = x(_),   % Temporary variable → X register
        V = vi(_,last,R).

analyze_multiple(VI, _, R) :=
    ->  R = y(_),   % Permanent variable → Y register
        analyze_permanent(VI, R).
```

The key insight: variables used across multiple goals need **Y registers** (permanent, environment-allocated), while variables local to a goal can use **X registers** (temporary, argument registers).

## Stage 3: Choice Point Generation

### Try/Retry/Trust Compilation (`try.akl:0-80`)

For predicates with multiple clauses, the compiler generates **indexing code** to efficiently select the applicable clause.

**Indexing Strategy:**

```prolog
compile_choice(Clauses, Modes, Used, Max)-S :-
    ->  Switch = switch_on_term(Var,Atm,Int,Flt,Big,Lst,Str),
        [label(M2)]-S,
        [Switch]-S,
        key_clause_pairs(Clauses, KC0),
        KC = [var-VC, atm-AC, int-IC, flt-FC, big-BC, lst-LC, str-SC|KC0],
        keysort_and_group(KC, KG),
        bind_group_headers(KG),
        compile_constant_group(AC, VC, Atm, atm, TC0, TC1)-S,
        compile_constant_group(IC, VC, Int, int, TC1, TC2)-S,
        compile_float_group(FC, Flt, TC2, TC3)-S,
        compile_constant_group(BC, VC, Big, big, TC3, TC4)-S,
        compile_list_group(LC, Lst, TC4, TC5)-S,
        compile_structure_group(SC, VC, Str, TC5, [])-S,
        % ... generate try/retry/trust chains
```

**Generated Code Pattern:**

For a predicate with multiple clauses:

```prolog
append([], Ys, Zs) := -> Zs = Ys.
append([X|Xs], Ys, Zs) := ->
    Zs = [X|Zs1],
    append(Xs, Ys, Zs1).
```

The compiler generates:

```
label(3)
    switch_on_term(5, 7, 7, 7, 7, 0, 7)  % Variable→5, List→0, else→7
label(5)
    try_single(1)   % Only one clause for variables
label(0)
    try(2)          % Two clauses for lists
    trust(1)
label(2)
    % First clause code
label(1)
    % Second clause code
label(7)
    % Fail
```

The `switch_on_term` instruction dispatches on the first argument's type:
- **Var** — unbound variable
- **Atm** — atom
- **Int** — integer
- **Flt** — float
- **Big** — bignum
- **Lst** — list
- **Str** — structure

## Stage 4: Head Compilation

### Head Code Generation (`head.akl:0-100`)

The head compiler generates **get_*** instructions to match/destructure the clause head against incoming arguments.

**Argument Reordering** (`head.akl:17-33`):

Following Carlsson's optimization, the compiler processes:
1. **Variable arguments** first (right-to-left)
2. **Complex arguments** second (left-to-right)

This minimizes register pressure and temporary allocations.

```prolog
reorder_head_args(Args, PrepArgs) :=
    reorder_head_args(Args, 0, P, P, PrepArgs).

reorder_head_args([var(U)|As], N, P0, V0, V) :-
    ->  V1 = [arg(N)-var(U)|V0],
        N1 is N+1,
        reorder_head_args(As, N1, P0, V1, V).
reorder_head_args([A|As], N, P0, V0, V) :=
    ->  P0 = [arg(N)-A|P1],
        N1 is N+1,
        reorder_head_args(As, N1, P1, V0, V).
```

**Code Generation** (`head.akl:45-80`):

```prolog
get_arg(var(U), R, C, T0, T, S0, S, V0, V, P0, P) :-
    ->  T = T0,
        occurrence(R, UI2, C, V0, V1),
        occurrence(U, UI1, C, V1, V),
        S0 = [get_v(UI1,UI2)|S],
        P0 = P.

get_arg(struct(F,Ar,As), R, C, T0, T, S0, S, V0, V, P0, P) :-
    ->  occurrence(R, UI, C, V0, V1),
        S0 = [get_s(F,Ar,UI)|S1],
        unify_get_args(As, C, T0, T1, S1, S2, V1, V2, SS, []),
        get_args(SS, C, T1, T, S2, S, V2, V, P0, P).
```

**Example:**

For the clause head `append([X|Xs], Ys, Zs)`:

```
get_list(0)              % Arg 0 must be a list
unify_x_variable(4,0)    % X = car, to X4
unify_x_variable(5,1)    % Xs = cdr, to X5
get_x_variable(1,1)      % Ys to X1
get_x_variable(2,2)      % Zs to X2
```

## Stage 5: Body Compilation

### Body Code Generation (`body.akl:0-100`)

The body compiler generates **put_*** and **call** instructions for goal invocation.

**Goal Classification:**

```prolog
goal_code(struct((=),2,[X,Y]), C0, C, T0, T, S0, S, V0, V, P0, P) :-
    ->  inline_unify(X, Y, C0, T0, T, S0, S, V0, V, _, P0, P),
        C = C0.

goal_code(struct(F,N,Args), C0, C, T0, T, S0, S, V0, V, P0, P) :-
    c_builtin(F,N)
    ->  C = C0,
        S1 = [call(F,N)|S],
        reorder_goal_args(Args, PrepArgs),
        put_args(PrepArgs, C0, T0, T, S0, S1, V0, V, P0, P).

goal_code(struct(F,N,Args), C0, C, T0, T, S0, S, V0, V, P0, P) :-
    ->  C is C0+1,
        S1 = [call(F,N)|S],
        reorder_goal_args(Args, PrepArgs),
        put_args(PrepArgs, C0, T0, T, S0, S1, V0, V, P0, P).
```

**Unification Inlining:**

Unifications (`X = Y`) are compiled inline rather than as calls to a unification predicate, using the **unify** module to generate specialized instruction sequences.

**put_arg Code Generation** (`body.akl:88-99`):

```prolog
put_arg(var(U), R, C, T0, T, S0, S, V0, V, P0, P) :-
    ->  T = T0,
        P = P0,
        occurrence(U, UI1, C, V0, V1),
        occurrence(R, UI2, C, V1, V),
        S0 = [put_v(UI1,UI2)|S].
```

**Example:**

For the body `append(Xs, Ys, Zs1)`:

```
put_x_value(5,0)     % Xs (from X5) to arg 0
put_x_value(1,1)     % Ys (from X1) to arg 1
put_x_value(6,2)     % Zs1 (from X6) to arg 2
call(append/3)       % Call append/3
```

## Stage 6: Clause Compilation

### Complete Clause Generation (`clause.akl:0-79`)

The clause module orchestrates the complete compilation of a clause:

```prolog
compile_clause(Cl, S0, S)-P :-
    clause_label(Cl, L),
    S0 = [label(L)|S1],
    raw_clause_code(Cl, C, VI)-P,
    analyze_varinfo(VI),
    allocate_registers(C, VI, Ysize),
    real_instructions(C, D),
    edit_and_peephole(D, Ysize, S1, S).

raw_clause_code(Cl, S0, V0)-P :-
    clause_head(Cl, Head),
    head_code(Head, T0, S0, S1, V0, V1)-P,
    C0 = 0,
    clause_guard(Cl, Guard),
    body_code(Guard, C0, C1, T0, T1, S1, S2, V1, V2)-P,
    C2 is C1+1,
    clause_op(Cl, Op),
    guard_op_code(Op, C2, S2, S3, V2, V3),
    C3 is C2+1,
    clause_body(Cl, Body),
    body_code(Body, C3, _C, T1, _T, S3, [], V3, [])-P.
```

**Guard Operator Code** (`clause.akl:30-57`):

```prolog
guard_op_code(Op, _, S0, S, V0, V) :=
    ->  V = V0,
        S0 = [guard_op(Op)|S].
```

This generates:
- `guard_cond` for `->` (conditional)
- `guard_commit` for `|` (commit)
- `guard_wait` for `?` (wait)
- `guard_cut` for `!` (cut)

## Stage 7: Register Allocation

### Register Assignment (`allocate.akl:0-79`)

The allocator assigns **X registers** (temporary) and **Y registers** (permanent) based on variable occurrence analysis.

**Permanent Register Allocation:**

```prolog
allocate_permanent([], Y, F) :=
    ->  Y = 0,
        F = [].
allocate_permanent([_-vi(_,last,y(I))|VI], Y, F) :-
    ->  allocate_permanent(VI, Y0, F0),
        allocate_permanent(F0, F, I, Y0, Y).
allocate_permanent([_-vi(_,first,y(I))|VI], Y, F) :-
    ->  allocate_permanent(VI, Y, F0),
        F = [I|F0].
allocate_permanent([_|VI], Y, F) :=
    ->  allocate_permanent(VI, Y, F).
```

This implements **stack slot allocation** for permanent variables, minimizing the environment size.

**Temporary Register Allocation** (`allocate.akl:35-78`):

```prolog
allocate_temporary(VI, C) :=
    max_argument_reg(C, M),
    allocate_temporary(VI, M, []).
```

Temporary variables use the **leftmost available X register** after argument registers, a simple but effective strategy for a single-pass allocator.

### Limitations

From the README:

> Allocation of y-registers has been improved. Allocation of x-registers is still very naive.

The X register allocator doesn't perform:
- **Graph coloring** for optimal register reuse
- **Lifetime analysis** for minimal register pressure
- **Register targeting** for call conventions

These optimizations were left for future work.

## Stage 8: Instruction Generation

### Concrete Instruction Emission (`instr.akl:0-80`)

The instruction generator converts abstract instructions (with occurrence info) into concrete PAM instructions (with register numbers):

```prolog
real_instruction(get_v(ri(first,x(I)),R), C0, C, S0, S) :-
    ->  C = C0,
        reg_or_ri(R, J),
        S0 = [get_x_variable(I,J)|S].

real_instruction(get_v(ri(_,x(I)),R), C0, C, S0, S) :-
    ->  C = C0,
        reg_or_ri(R, J),
        S0 = [get_x_value(I,J)|S].

real_instruction(get_v(ri(first,y(I)),R), C0, C, S0, S) :-
    ->  C = C0,
        reg_or_ri(R, J),
        S0 = [get_y_variable(I,J)|S].
```

**Transformation Examples:**

```
Abstract:                   Concrete:
get_v(ri(first,x(3)), 0)   → get_x_variable(3,0)
get_v(ri(last,x(3)), 1)    → get_x_value(3,1)
get_s(append,3,0)          → get_structure(append/3, 0)
put_v(ri(first,y(2)), 1)   → put_y_variable(2,1)
```

## Stage 9: Peephole Optimization

### Edit and Optimize (`edit.akl`)

The edit module performs **peephole optimizations**:

- **Instruction fusion** — combining consecutive instructions
- **Dead code elimination** — removing unreachable code
- **Jump threading** — optimizing jump chains
- **Redundant move elimination** — removing unnecessary register transfers

Common patterns optimized:

```
put_x_variable(I,I)  →  (eliminated - redundant)
get_x_value(I,I)     →  (eliminated - redundant)
jump(L1); label(L1)  →  label(L1)
```

The optimizations are conservative, preserving semantics while reducing instruction count and improving cache utilization.

## Stage 10: PAM Output

### Writing Bytecode (`writepam.akl:0-100`)

The final stage emits PAM bytecode in textual format:

```prolog
write_pam(pam(Name/Arity,Instrs))-S :-
    atom(Name)
    ->  puts('predicate(')@S,
        putsq(Name,0'')@S,
        puts('/')@S,
        putd(Arity)@S,
        putc(10)@S,
        write_instrs(Instrs)-S,
        putc(10)@S,
        putc(10)@S.
```

**Output Format:**

```
predicate('append'/3
label(3)
    switch_on_term(5,5,5,5,5,0,5)
label(5)
    try_single(1)
label(0)
    try(2)
    trust(1)
label(2)
    allocate(3)
    get_nil(0)
    get_x_variable(1,1)
    get_x_variable(2,2)
    guard_wait
    unify_x_variable(1,2)
    dealloc_proceed
label(1)
    allocate(5)
    get_list(0)
    unify_x_variable(3,0)
    unify_x_variable(4,1)
    get_x_variable(1,1)
    get_list(2)
    unify_x_value(3,0)
    unify_x_variable(5,1)
    guard_wait
    put_x_value(4,0)
    put_x_value(1,1)
    put_x_value(5,2)
    dealloc_execute('append'/3)
)
```

This format is loaded by the emulator's bytecode loader and converted to internal instruction structures.

## Complete Compilation Example

Let's trace the compilation of a simple predicate:

### Input (`member.akl`):

```prolog
member(X, [X|_]) := -> true.
member(X, [_|Xs]) := -> member(X, Xs).
```

### Stage 1: Parsing

```prolog
Definitions = [
    def(member/2, [
        clause(
            struct(member, 2, [var('X'), struct('.', 2, [var('X'), var('_')])]),
            [],  % guard
            cond,  % operator
            [struct(true, 0, [])]
        ),
        clause(
            struct(member, 2, [var('X'), struct('.', 2, [var('_'), var('Xs')])]),
            [],
            cond,
            [struct(member, 2, [var('X'), var('Xs')])]
        )
    ])
]
```

### Stage 2: Analysis

```prolog
Clause 1 variables:
    X: [vi(0, first, x(0)), vi(0, middle, x(0))]  → x(0)
    _: vi(0, void, none)                           → none

Clause 2 variables:
    X: [vi(0, first, x(0)), vi(1, last, x(0))]    → x(0)
    _: vi(0, void, none)                           → none
    Xs: [vi(0, first, x(1)), vi(1, last, x(1))]   → x(1)
```

### Stage 3: Choice Point Generation

```
switch_on_term(Var, Atm, Int, Flt, Big, Lst, Str)
```

Both clauses require a list as second argument, so indexing dispatches to list processing.

### Stage 4-6: Code Generation

**Clause 1:**

```
label(1)
    get_x_variable(0, 0)      % X from arg 0
    get_list(1)               % Arg 1 must be list
    unify_x_value(0, 0)       % Car must equal X
    unify_void(1)             % Ignore cdr
    guard_cond
    proceed
```

**Clause 2:**

```
label(2)
    get_x_variable(0, 0)      % X from arg 0
    get_list(1)               % Arg 1 must be list
    unify_void(1)             % Ignore car
    unify_x_variable(1, 1)    % Xs = cdr, to X1
    guard_cond
    put_x_value(0, 0)         % X to arg 0
    put_x_value(1, 1)         % Xs to arg 1
    execute(member/2)         % Tail call
```

### Stage 7: Try/Retry/Trust

```
predicate('member'/2
label(0)
    switch_on_term(3,3,3,3,3,1,3)
label(3)
    fail
label(1)
    try(2)
    trust(4)
label(2)
    % Clause 1 code
label(4)
    % Clause 2 code
)
```

### Stage 8-10: Final Output

After optimization and formatting, the final `.pam` file contains the bytecode ready for the emulator to load.

## Specialized Compilation

### Finite Domain Constraints (`fd_akl.akl`, `fd_arith_*.akl`)

The compiler includes specialized modules for finite domain (FD) constraint compilation:

- **fd_akl.akl** — FD constraint recognition and delegation
- **fd_arith_ask.akl** — Arithmetic constraint queries (X #< Y)
- **fd_arith_tell.akl** — Arithmetic constraint assertions
- **fd_arith_compile.akl** — Code generation for FD operations
- **fd_mon.akl** — Monotonic FD operations
- **fd_range.akl** — Domain range operations

These modules generate specialized `fd_code(Reg, Code)` instructions that the emulator dispatches to the FD constraint solver.

### Abstraction Compilation

Lambda abstractions (`\Args.Body`) are compiled to numbered predicates:

```prolog
compile_abstraction(abs(As,_Fs,Es,Body,Label), Modes, N0, N, P0, P):-
    ->  '$concat'(As, Es, Arg),
        length(Es, E),
        length(Arg, A),
        Label = N0,
        inc(N0, N1),
        conj_to_list(Body, Bs),
        Clauses = [Clause],
        clause_head(Clause, struct(abs, A, Arg)),
        clause_guard(Clause, []),
        clause_op(Clause, cond),
        clause_body(Clause, Bs),
        compile_choice(Clauses, Modes, Used, Max, C0, C1),
        compile_clauses(Clauses, Modes, Used, Max, C1, [], Int, []),
        compile_abstractions(Int, Modes, N1, N, P0, P1),
        P1 = [pam(N0/A/E, C0)|P].
```

Abstractions become predicates named `N/A/E` where:
- **N** — unique number
- **A** — total arity (abstraction parameters + extra arguments)
- **E** — number of extra arguments

Example:
```prolog
map(\X.f(X), [1,2,3], Ys)
```

The abstraction `\X.f(X)` compiles to predicate `42/1/0`, and `map` receives `42/1/0` as an argument for `apply/2`.

## Bootstrapping

The compiler is self-hosting: it's written in AKL and compiles itself. This creates a **bootstrapping problem**:

1. You need a compiler to compile AKL programs
2. But the compiler is an AKL program
3. Therefore, you need a compiler to compile the compiler

### Solution: Pre-compiled Bootstrap

The system includes `environment/boot.pam` (~397KB), a pre-compiled version of:
- The compiler (`oldcompiler/comp.pam`)
- Standard library predicates (`environment/*.akl`)
- Built-in definitions (`environment/builtin.akl`)

**Bootstrap Sequence:**

```
1. Emulator starts
2. Load boot.pam (includes compiler)
3. Compiler now available as compilef/1, compile/1
4. User can compile new programs
5. User can recompile the compiler itself
```

**Recompiling the Compiler:**

```bash
cd oldcompiler
make clean
make
```

This:
1. Uses the current `../agents` emulator
2. Loads `boot.pam` (previous compiler)
3. Compiles each `.akl` file to `.pam`
4. Concatenates all `.pam` files into `comp.pam`
5. New `comp.pam` can replace the compiler in `boot.pam`

### Bootstrap Integrity

The bootstrap is "three-point": three versions must be consistent:

1. **Emulator** (`emulator/agents`) — executes bytecode
2. **Compiler** (`oldcompiler/comp.pam`) — generates bytecode
3. **Source** (`oldcompiler/*.akl`) — compiler source code

Changes to any one require:
- Recompiling with the old compiler
- Verifying the new compiler works
- Recompiling with the new compiler (fixed point)

## Performance Characteristics

### Compilation Speed

The AKL compiler is notably **slower** than optimized Prolog compilers due to:

- **Interpreted execution** — Compiler runs on the bytecode interpreter, not native code
- **Naive X register allocation** — No graph coloring or sophisticated optimization
- **No caching** — Each compilation starts from source
- **Concurrent overhead** — Port-based I/O and concurrent goals add coordination cost

Typical compilation times (early 1990s SPARC):
- Simple predicate (< 10 clauses): 100-500 ms
- Medium module (50-100 clauses): 1-5 seconds
- Large module (500+ clauses): 10-60 seconds
- Compiler itself (3500 lines): ~2 minutes

Modern 64-bit systems are 10-50× faster, but still slower than native-compiled Prolog compilers.

### Code Quality

The generated PAM bytecode quality:

**Strengths:**
- **Good indexing** — Efficient switch_on_term dispatch
- **Inline unification** — Specialized match instructions
- **Tail call optimization** — Execute instead of call+proceed
- **Improved Y allocation** — Minimal environment size

**Weaknesses:**
- **Naive X allocation** — Poor register reuse
- **No instruction scheduling** — Missed parallelism opportunities
- **Limited peephole scope** — Only local optimizations
- **No whole-program analysis** — Each predicate compiled independently

Compared to WAM Prolog compilers (like SICStus or SWI-Prolog), AGENTS bytecode is **2-5× larger** and **1.5-3× slower** due to less aggressive optimization.

## Comparison with Other Systems

### WAM Prolog Compilers

| Feature | AGENTS Compiler | WAM Prolog |
|---------|----------------|------------|
| **Implementation Language** | AKL (self-hosting) | C/Prolog |
| **Compilation Model** | Multi-pass pipeline | Typically single-pass |
| **Register Allocation** | Naive X, improved Y | Graph coloring |
| **Indexing** | switch_on_term | Hash tables, tries |
| **Optimizations** | Peephole only | Inline, specialize, JIT |
| **Guard Support** | Native (4 operators) | Via cuts/conditionals |
| **Constraint Compilation** | Integrated FD | External libraries |
| **Bootstrap** | Pre-compiled PAM | Native executable |

### Concurrent Logic Languages

AGENTS shares compilation challenges with other concurrent logic languages:

- **GHC (Guarded Horn Clauses)** — Similar guard compilation, simpler suspension
- **Parlog** — Parallel execution, different commitment model
- **KL1** — Parallel inference machine, specialized hardware
- **Oz** — Full dataflow, more complex compilation

AGENTS' approach is notable for:
- **Deep guards** — Backtracking within guards, rare in concurrent logic languages
- **Self-hosting** — Most concurrent logic compilers written in Prolog or C
- **Simplicity** — 3500 lines vs. 10,000+ for comparable systems

## Extending the Compiler

### Adding New Instructions

To add a new PAM instruction:

**1. Define abstract instruction form** (e.g., in `body.akl`):

```prolog
goal_code(struct(my_builtin,1,[Arg]), C0, C, T0, T, S0, S, V0, V, P0, P) :-
    ->  C = C0,
        occurrence(Arg, UI, C0, V0, V),
        S0 = [my_instruction(UI)|S],
        T = T0,
        P = P0.
```

**2. Add concrete instruction generation** (in `instr.akl`):

```prolog
real_instruction(my_instruction(ri(_,x(I))), C0, C, S0, S) :-
    ->  C = C0,
        S0 = [my_concrete_instr(I)|S].
```

**3. Implement emulator handler** (in `emulator/engine.c`):

```c
CaseLabel(MY_CONCRETE_INSTR):
{
    Instr_One(MY_CONCRETE_INSTR, reg);
    // Implementation
    NextOp();
}
```

**4. Update instruction tables** (in `emulator/opcodes.h`):

```c
Item(MY_CONCRETE_INSTR)
```

**5. Recompile compiler and emulator:**

```bash
cd oldcompiler && make
cd ../emulator && make
```

### Adding Compiler Optimizations

To add a new optimization pass:

**1. Create optimization module** (e.g., `myopt.akl`):

```prolog
my_optimization(CodeIn, CodeOut) :=
    ->  analyze_code(CodeIn, Info),
        transform_code(CodeIn, Info, CodeOut).
```

**2. Integrate into pipeline** (in `clause.akl`):

```prolog
compile_clause(Cl, S0, S)-P :-
    clause_label(Cl, L),
    S0 = [label(L)|S1],
    raw_clause_code(Cl, C, VI)-P,
    analyze_varinfo(VI),
    allocate_registers(C, VI, Ysize),
    real_instructions(C, D),
    my_optimization(D, D1),  % ADD HERE
    edit_and_peephole(D1, Ysize, S1, S).
```

**3. Update Makefile:**

```make
SRC = ... myopt.akl ...
OBJ = ... myopt.pam ...
```

**4. Recompile:**

```bash
cd oldcompiler && make
```

## Debugging the Compiler

### Compilation Stages

The compiler's modular structure allows debugging individual stages. Set compilation flags in `start.akl` (in the experimental `compiler/` directory) or use the REPL:

```prolog
?- compilef(myfile).   % Normal compilation

?- compile(myfile, []).  % With mode flags

?- % Trace specific stages:
   read_definitions(Defs) @In,
   compile_definitions(Defs, [], []) @Out.
```

### Common Issues

**1. Variable Occurrence Mismatch:**

```
ERROR: Variable X has void occurrence but is used in goal 5
```

**Fix:** Check `analyze.akl:occurrence/5` for incorrect void classification.

**2. Register Allocation Failure:**

```
ERROR: Insufficient X registers for clause (need 20, have 16)
```

**Fix:** Increase `NUMREGS` in `emulator/config.h`, or simplify the clause.

**3. Invalid Instruction Sequence:**

```
ERROR: get_x_value before get_x_variable for X3
```

**Fix:** Check argument reordering in `head.akl:reorder_head_args/2`.

**4. Label Resolution Error:**

```
ERROR: Undefined label L42 in predicate foo/3
```

**Fix:** Check `try.akl` for correct label generation in indexing code.

## Future Improvements

The README notes that X register allocation is "still very naive." Potential enhancements:

### Better Register Allocation

**Graph Coloring:**

Replace the current leftmost-available strategy with graph coloring:

1. Build **interference graph** of variable lifetimes
2. Color graph with available X registers
3. Spill to Y registers on conflicts

Expected improvement: **20-30% fewer X registers**, reducing environment size and improving cache utilization.

**Lifetime Analysis:**

Track precise variable lifetimes to enable:
- Early register reclamation
- Better register targeting for calls
- Reduced environment creation

### Advanced Optimizations

**Inline Expansion:**

Inline small predicates at call sites:

```prolog
% Before:
foo(X) := bar(X, Y), baz(Y).
bar(A, B) := B is A + 1.

% After inlining bar:
foo(X) := Y is X + 1, baz(Y).
```

**Partial Evaluation:**

Specialize predicates for known argument patterns:

```prolog
% Generic:
append([], Ys, Ys).
append([X|Xs], Ys, [X|Zs]) := append(Xs, Ys, Zs).

% Specialized for second argument known:
append_to_list123([], [1,2,3]).
append_to_list123([X|Xs], [X,1,2,3|Rest]) :=
    append_to_list123(Xs, Rest).
```

**Common Subexpression Elimination:**

Factor out repeated computations across clauses.

### Better Indexing

**Hash-Based Indexing:**

Replace linear switch tables with hash tables for large constant sets.

**Trie Indexing:**

Use decision tries for multi-argument indexing (SWI-Prolog's JIT indexing).

**Dynamic Recompilation:**

Recompile predicates based on runtime call patterns (HotSpot-style adaptive optimization).

## Summary

The AGENTS compiler demonstrates the **expressiveness and power** of the AKL language by implementing a complete, self-hosting compiler in pure constraint logic code. Key achievements include:

1. **Multi-stage pipeline** — Clean separation of concerns (parsing, analysis, code generation, optimization)
2. **Constraint-based design** — Variable occurrence analysis and register allocation using logic programming
3. **Self-hosting** — Written entirely in the language it compiles (3,500 lines of AKL)
4. **Guard compilation** — Native support for four guard operators (cond, commit, wait, cut)
5. **FD integration** — Specialized compilation for finite domain constraints
6. **Bootstrap capability** — Pre-compiled bytecode enables system self-sufficiency

While the compiler's optimization is modest compared to industrial Prolog systems, its **clarity, modularity, and self-hosting nature** make it an excellent example of logic programming applied to compiler construction.

The next chapter examines the **debugging and tracing system**, which provides visibility into program execution and helps developers understand the complex interaction of guards, choice points, and concurrent execution in AKL programs.
