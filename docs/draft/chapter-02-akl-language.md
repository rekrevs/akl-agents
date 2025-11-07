# Chapter 2: The AKL Language

## Introduction

To understand the AGENTS implementation, we must first understand what it implements: the **Andorra Kernel Language (AKL)**. This chapter presents AKL from a practical perspective, showing enough of the language to make sense of the implementation decisions in later chapters.

We will not provide a complete formal semantics (see Appendix F for references to the academic literature). Instead, we focus on the operational understanding needed by implementers: what constructs exist, how they behave, and what challenges they present for implementation.

## Syntax Overview

AKL syntax resembles Prolog but with important extensions. The basic building block is a **clause**:

```
head :- guard ? body.
```

Where:
- **head**: A term matching goals that call this clause
- **guard**: A goal that must succeed for the clause to be selected
- **body**: Goals to execute if the guard commits
- **:-** separates head from guard
- **?** separates guard from body (guard commitment operator)

### Simplified Forms

When the guard is trivial, it can be omitted:

```
head :- body.
```

This is syntactic sugar for `head :- true ? body.`

When there's no body:

```
head :- guard ? .
```

Or simply:

```
head.
```

A fact with no body or guard.

## Data Types

AKL inherits Prolog's type system with some extensions.

### Atoms

Atoms are symbolic constants:

```
apple
'Hello World'
[]
```

The empty list `[]` is an atom. Atoms starting with lowercase letters or enclosed in quotes.

### Numbers

**Small integers** fit in a machine word (58 bits on 64-bit platforms):

```
42
-17
0
```

**Bignums** (arbitrary precision) via GMP (excluded in modern 64-bit builds):

```
123456789012345678901234567890
```

**Floating point** numbers:

```
3.14159
1.0e-10
```

**Rationals** (fractions):

```
2/3
-5/7
```

### Structures

Compound terms with a functor and arguments:

```
point(10, 20)
person('Alice', 30, engineer)
node(left(leaf), right(branch(...)))
```

The functor `f` with arity `n` is written `f/n` in documentation.

### Lists

Lists are syntactic sugar for structures:

```
[1, 2, 3]           % Sugar for [1|[2|[3|[]]]]
[H|T]               % Head-tail decomposition
[A, B | Rest]       % First two elements plus rest
```

Internally, `[H|T]` is the structure `.(H, T)` with functor `./2`.

### Variables

Variables start with uppercase letters or underscore:

```
X
_Result
_       % Anonymous variable (each occurrence is distinct)
```

Variables can be:
- **Unbound unconstrained**: Not yet assigned a value
- **Unbound constrained**: Has constraints but not yet bound
- **Bound**: Unified with a specific term

### Ports

A **port** is a special kind of stream with a write-protected tail:

```
port(P)             % Create a port P
send(Message, P)    % Send message to port (non-blocking)
```

Ports are represented as lists, but only the port creator can extend the tail.

## Clauses and Predicates

### Basic Clauses

A **predicate** is defined by one or more clauses:

```
% Factorial
fact(0, 1).
fact(N, F) :=
    N > 0,
    N1 is N - 1,
    fact(N1, F1),
    F is N * F1.
```

This looks like Prolog. The first clause is a fact, the second a rule.

### Multiple Clauses

When multiple clauses exist, they represent alternatives (or-parallelism):

```
member(X, [X|_]).
member(X, [_|Xs]) := member(X, Xs).
```

If the first clause fails to unify, the second is tried. This is **don't-know nondeterminism**.

### Guarded Clauses

Guards add synchronization and commitment:

```
% Producer-consumer with guard
consume(X, [H|T]) :=
    H = value ?          % Guard waits for H to be bound
    process(H),
    consume(X, T).
```

The **guard** `H = value` must succeed before the clause commits. If `H` is unbound, the guard **suspends** until `H` becomes bound.

## Guards: Synchronization and Commitment

### Guard Semantics

A guard is a goal that:
1. **Asks** the constraint store if it can succeed
2. If yes, the clause **commits** (no backtracking to other clauses)
3. If no, try the next clause
4. If uncertain (variables unbound), **suspend** until more information arrives

### The Commitment Operator (?)

The `?` operator marks **commitment**:

```
clause1(X) := guard1 ? body1.
clause2(X) := guard2 ? body2.
```

Execution:
1. Try `guard1`
   - If it succeeds, **commit** to `body1` (no backtracking)
   - If it fails, try `clause2`
   - If it suspends, wait
2. Try `guard2` similarly

Once a guard succeeds and commits, there's no going back to try other clauses. This is **don't-care nondeterminism**.

### Backtracking Within Guards

Crucially, you can **backtrack inside a guard**:

```
member_guard(X, L) :=
    member(X, L) ?       % Try all members until one succeeds
    process(X).
```

The guard `member(X, L)` can backtrack through all list elements. But once a member is found and the guard succeeds, we commit—we won't backtrack to try other members even if `process(X)` fails.

This combination of search (within guards) and commitment (across guards) is powerful and unique to AKL.

### Guard Suspension

Guards enable **synchronization**:

```
% Wait for both arguments to be bound
both_bound(X, Y) :=
    integer(X), integer(Y) ?
    compute(X, Y).
```

If `X` or `Y` is unbound, the guard suspends. When they're both bound, the guard succeeds and commits.

This is how AKL achieves **dataflow synchronization** without explicit wait operations.

### Trivial Guards

Many clauses have trivial guards that always succeed:

```
append([], Ys, Ys).
append([X|Xs], Ys, [X|Zs]) := append(Xs, Ys, Zs).
```

These are really:

```
append([], Ys, Ys) := true ? .
append([X|Xs], Ys, [X|Zs]) := true ? append(Xs, Ys, Zs).
```

The guard `true` always succeeds immediately, so these behave like Prolog clauses.

## Concurrent Execution: And-Parallelism

### Conjunctions as Concurrent Agents

In AKL, a conjunction of goals can execute **concurrently**:

```
solve(X, Y) :=
    subproblem1(X),      % Agent 1
    subproblem2(Y).      % Agent 2
```

Both `subproblem1(X)` and `subproblem2(Y)` can execute in parallel. They communicate by binding shared variables.

This is **and-parallelism** (parallel conjunction) as opposed to **or-parallelism** (parallel disjunction/backtracking).

### Synchronization Via Shared Variables

Agents synchronize by reading and writing shared variables:

```
producer_consumer :=
    producer(Stream),
    consumer(Stream).

producer([Item1, Item2, Item3 | Rest]) :=
    generate_items([Item1, Item2, Item3 | Rest]).

consumer([H|T]) :=
    process(H),
    consumer(T).
```

The `consumer` suspends when it tries to unify with an unbound tail, waiting for the `producer` to extend the stream.

### Sequential vs. Parallel

Whether conjunctions execute sequentially or in parallel is an implementation choice. AGENTS is a **sequential** implementation—it simulates concurrency by interleaving agent execution. A parallel implementation would execute agents on multiple processors.

The semantics are the same: agents can be thought of as running concurrently, communicating through shared logical variables.

## Port-Based Communication

### Creating Ports

A **port** is created with `open_port/2`, which connects a **port** (bag) to a **stream** (list):

```
setup_communication(P, S) :=
    open_port(P, S).
```

This creates port `P` and associates it with stream `S`. Messages sent to port `P` appear on stream `S`.

### Sending Messages

Messages are sent with `send/2`:

```
send_data(Message, Port) :=
    send(Message, Port).
```

**Key properties:**
- `send/2` is **non-blocking** (always succeeds immediately)
- Messages appear on the associated stream in FIFO order
- Constant message delivery delay

### Receiving Messages

Receive by pattern-matching on the stream:

```
receive_loop(S) :=
  ( S = [Msg|Rest] →
      process(Msg),
      receive_loop(Rest)
  ; S = [] →
      true ).
```

If the stream tail is unbound, the receiver suspends until a message arrives.

### Automatic Stream Closure

When the port is garbage collected (no more references to it), the associated stream is automatically closed. This provides automatic termination detection.

### Why Ports?

Ports provide **asynchronous communication** with automatic termination detection. They are constraint-based abstractions connecting bags (ports) to streams, not simple variables. In earlier concurrent logic languages like GHC, all communication was via shared variables, which can lead to complex synchronization patterns. Ports give a cleaner model for message passing.

### Example: Producer-Consumer with Ports

```
main :=
    producer(5, S),
    consumer(S).

% Producer creates port and sends N messages
producer(N, S) :=
    open_port(P, S),
    prod_loop(N, P).

prod_loop(N, P) :=
  ( N > 0 →
      send(N, P),
      N1 = N - 1,
      prod_loop(N1, P)
  ; true ).

% Consumer receives from stream
consumer(S) :=
  ( S = [Item|Rest] →
      process(Item),
      consumer(Rest)
  ; S = [] →
      write('done') ).
```

When `prod_loop` terminates, port `P` becomes garbage and stream `S` is automatically closed.

The producer sends messages asynchronously. The consumer processes them as they arrive. Clean and efficient.

## Constraint Solving

### Constraint Stores

Computation in AKL operates on **constraint stores**. A store contains:
- Variable bindings
- Constraints on variables

Agents **tell** constraints (add to store) and **ask** questions (query the store).

### Equality Constraints

Basic unification is equality constraint solving:

```
X = Y                   % Tell: X equals Y
X = f(a, b)            % Tell: X is the structure f(a,b)
```

When we unify `X` with `Y`, we add the constraint "X = Y" to the store. The store maintains consistency—if `X = Y` and `Y = 3`, then `X = 3`.

### Rational Trees

AKL supports **rational trees** (infinite structures via cycles):

```
X = f(X)               % X is a cyclic structure
```

This is allowed. The occurs check is optional.

### Finite Domain Constraints

AGENTS provides **finite domain (FD) constraints** over integers:

```
fd(
    X in 1..10,         % X is in the range 1 to 10
    Y in 1..10,
    X < Y,              % X is less than Y
    X + Y =:= 15        % X plus Y equals 15
)
```

FD constraints enable **constraint-based search** for combinatorial problems.

### FD Constraint Syntax

Inside an `fd(...)` block, we can use:

**Domain constraints:**
```
X in 1..100            % X in range
X in {1, 5, 9}         % X in set
X in dom(Y)            % X in same domain as Y
```

**Arithmetic constraints:**
```
X =:= Y + Z            % Equality
X < Y                  % Less than
X =< Y                 % Less than or equal
X \= Y                 % Not equal
```

**Boolean combinations:**
```
X in dom(Y) /\ dom(Z)  % Intersection
X in dom(Y) \/ dom(Z)  % Union
X in -dom(Y)           % Complement
```

### Example: N-Queens

The classic N-Queens problem in AKL with FD:

```
queens(N, L) :=
    construct_domains(N, L),    % L is list of FD variables
    constrain(L),                % Post constraints
    label(L).                    % Search for solution

construct_domains(0, []).
construct_domains(N, [D|Rest]) :=
    N > 0,
    fd(D in 0..N),               % Queen in column 0..N
    N1 is N - 1,
    construct_domains(N1, Rest).

constrain([]).
constrain([D|Rest]) :=
    constrain_each(Rest, D, 1),
    constrain(Rest).

constrain_each([], _, _).
constrain_each([E|Rest], D, S) :=
    fd(
        D \= E,                  % Not same column
        D \= E + S,              % Not same diagonal
        D \= E - S               % Not same diagonal
    ),
    S1 is S + 1,
    constrain_each(Rest, D, S1).

label([]).
label([D|Rest]) :=
    fd_labeling([D]),            % Assign value to D
    label(Rest).
```

The FD constraints propagate, pruning the search space. `fd_labeling/1` searches for a solution.

### Constraint Propagation

When you post a constraint, the system performs **constraint propagation**:

```
fd(X in 1..10, Y in 5..15, X =:= Y)
```

Propagation infers:
- `X in 1..10 ∩ 5..15 = 5..10`
- `Y in 5..15 ∩ 1..10 = 5..10`

The domains shrink without search. This is the power of constraint programming.

### Constraint Suspension

If a constraint can't be evaluated yet, it **suspends** on variables:

```
fd(X + Y =:= 10)
```

If both `X` and `Y` are unbound, this suspends. When either is bound, the constraint wakes and propagates.

## Control Flow

### Sequential Execution

Goals in a conjunction execute in an order determined by the implementation. In AGENTS (a sequential system), this is typically left-to-right:

```
goal1, goal2, goal3.
```

Executes `goal1`, then `goal2`, then `goal3`.

### Choice Points

Multiple clauses create choice points (or-parallelism):

```
member(X, [X|_]).
member(X, [_|Xs]) := member(X, Xs).
```

On backtracking, alternative clauses are tried.

### Cuts

The **cut** `!` commits to the current clause and discards choice points:

```
max(X, Y, X) := X >= Y, !.
max(X, Y, Y).
```

If `X >= Y`, we commit to the first clause. The cut removes the choice point for the second clause.

Cuts are useful for efficiency and expressing determinism, but they break logical purity.

### Negation

**Negation as failure** is supported:

```
not_member(X, L) := member(X, L), !, fail.
not_member(X, L).
```

Or using `\+`:

```
not_member(X, L) := \+ member(X, L).
```

This is **not logical negation**—it's procedural. If `member(X, L)` fails, `\+ member(X, L)` succeeds.

### If-Then-Else

Conditional execution:

```
max(X, Y, Max) :=
    ( X >= Y ->
        Max = X
    ;
        Max = Y
    ).
```

Syntax: `( Condition -> Then ; Else )`

If `Condition` succeeds, execute `Then` (and don't try `Else`). If `Condition` fails, execute `Else`.

## Higher-Order Constructs

### Aggregation

Collect all solutions to a goal:

```
all_ancestors(Person, Ancestors) :=
    bagof(A, ancestor(Person, A), Ancestors).
```

This finds all `A` such that `ancestor(Person, A)` and collects them into a list.

### numberof/2

Count solutions:

```
count_solutions(Goal, N) := numberof(Goal, N).
```

### Apply

Apply a term as a goal:

```
apply_goal(Goal) := apply(Goal).
```

If `Goal = append([1,2], [3,4], X)`, then `apply(Goal)` executes it.

## Built-in Predicates

AKL provides many built-ins for practical programming.

### Arithmetic

```
X is Y + Z             % Arithmetic evaluation
X > Y                  % Comparison
X =:= Y                % Arithmetic equality (evaluates)
X = Y                  % Unification (does not evaluate)
```

### Type Checking

```
var(X)                 % X is an unbound variable
nonvar(X)              % X is not an unbound variable
atom(X)                % X is an atom
integer(X)             % X is an integer
number(X)              % X is a number
compound(X)            % X is a structure
```

### List Operations

```
append(L1, L2, L3)     % List concatenation
length(L, N)           % List length
member(X, L)           % List membership
```

### I/O

```
write(Term)            % Write term to output
nl                     % Write newline
read(Term)             % Read term from input
get_char(Char)         % Read character
put_char(Char)         % Write character
```

### Meta-Programming

```
functor(Term, F, N)    % Get functor and arity
arg(N, Term, Arg)      % Get Nth argument
=..(Term, List)        % Univ: Term ↔ [functor|args]
```

### Ports

```
port(P)                % Create a new port
send(Msg, P)           % Send message to port (non-blocking)
```

### Constraint Solving

```
fd(Constraints)        % Finite domain constraints
fd_labeling(Vars)      % Search for FD solution
```

## Examples

### Example 1: Factorial (Prolog-like)

```
factorial(0, 1).
factorial(N, F) :=
    N > 0,
    N1 is N - 1,
    factorial(N1, F1),
    F is N * F1.
```

Straightforward recursion. No concurrency or constraints.

### Example 2: Concurrent Stream Processing

```
pipeline(Input, Output) :=
    stage1(Input, Intermediate),
    stage2(Intermediate, Output).

stage1([], []).
stage1([X|Xs], [Y|Ys]) :=
    process1(X, Y),
    stage1(Xs, Ys).

stage2([], []).
stage2([X|Xs], [Y|Ys]) :=
    process2(X, Y),
    stage2(Xs, Ys).
```

`stage1` and `stage2` run concurrently, communicating via the `Intermediate` stream. As `stage1` produces items, `stage2` consumes them. This is **pipelined parallelism**.

### Example 3: Producer-Consumer with Port

```
main(Sum) :=
    producer(5, S),
    consumer(S, Sum).

producer(N, S) :=
    open_port(P, S),
    prod_loop(N, P).

prod_loop(0, P) := send(done, P).
prod_loop(N, P) :=
    N > 0,
    send(N, P),
    N1 is N - 1,
    prod_loop(N1, P).

consumer(S, Sum) := consume(S, 0, Sum).

consume([done], Acc, Acc).
consume([N|Rest], Acc, Sum) :=
    Acc1 is Acc + N,
    consume(Rest, Acc1, Sum).
```

The producer sends numbers 5, 4, 3, 2, 1, done. The consumer sums them. Communication is asynchronous via the port.

### Example 4: N-Queens with Finite Domains

```
queens(N, Solution) :=
    length(Solution, N),
    domain(Solution, 1, N),
    safe(Solution),
    fd_labeling(Solution).

domain([], _, _).
domain([V|Vs], Min, Max) :=
    fd(V in Min..Max),
    domain(Vs, Min, Max).

safe([]).
safe([Q|Qs]) := safe(Qs), no_attack(Q, Qs, 1).

no_attack(_, [], _).
no_attack(Q, [Q2|Qs], Dist) :=
    fd(
        Q \= Q2,
        Q \= Q2 + Dist,
        Q \= Q2 - Dist
    ),
    Dist1 is Dist + 1,
    no_attack(Q, Qs, Dist1).
```

This uses FD constraints to prune the search space efficiently. For N=8, instead of 8^8 = 16 million possibilities, FD reduces it to 92 solutions with minimal search.

### Example 5: Guards for Synchronization

```
merge([], Ys, Ys).
merge(Xs, [], Xs).
merge([X|Xs], [Y|Ys], [X|Zs]) :=
    X =< Y ?
    merge(Xs, [Y|Ys], Zs).
merge([X|Xs], [Y|Ys], [Y|Zs]) :=
    Y < X ?
    merge([X|Xs], Ys, Zs).
```

The guards `X =< Y` and `Y < X` wait until both `X` and `Y` are bound before committing. This is **demand-driven** computation—we don't proceed until we have enough information.

## Differences from Prolog

While AKL resembles Prolog, key differences exist:

### 1. Guards and Commitment

Prolog has no guard syntax. Every clause is tried until one succeeds. Backtracking can revisit earlier clauses.

AKL guards commit: once a guard succeeds, you can't backtrack to try other clauses (though you can backtrack within the body).

### 2. Concurrency

Prolog is inherently sequential. Conjunctions execute left-to-right.

AKL allows concurrent execution of conjunctions (though AGENTS is a sequential implementation that could be parallelized).

### 3. Ports

Prolog has no port concept. Communication is via shared variables only.

AKL adds ports for asynchronous message passing.

### 4. Constraint Domains

Standard Prolog has no FD constraints. You'd use libraries like CLP(FD) in modern Prolog systems.

AGENTS integrates FD constraints as a core feature.

### 5. Module System

Prolog has various module systems (ISO modules, SWI modules, etc.).

AGENTS has a different module/package system (not covered in detail here).

### 6. Built-ins

Many Prolog built-ins are similar but not identical in AGENTS. For example:
- No `assert/retract` (predicates are immutable)
- Different I/O predicates
- Different module system

### 7. Operational Semantics

Prolog's operational semantics are well-defined (SLD resolution with backtracking).

AKL's semantics involve and-parallelism, or-parallelism, guards, suspension, and constraint propagation—more complex but also more expressive.

## Implementation Challenges

From a language design perspective, AKL presents several challenges for implementation:

### 1. Concurrent State Management

Supporting and-parallelism requires managing multiple execution states concurrently. The implementation must maintain:
- Multiple and-boxes (concurrent agents)
- Multiple choice-boxes (or-parallelism)
- Synchronization via constraints
- Efficient context switching

### 2. Guard Suspension

Guards that suspend on unbound variables require a **suspension mechanism**:
- Track which guards are waiting on which variables
- Wake guards when variables are bound
- Avoid busy-waiting or polling

### 3. Constraint Propagation

FD constraints require:
- Efficient domain representation
- Incremental propagation algorithms
- Suspension and waking of constraints
- Garbage collection of constraint data structures

### 4. Port Implementation

Ports need:
- Protected tail (only sender can extend)
- Efficient queuing
- Integration with suspension (receiver waits)
- Garbage collection (when no references remain)

### 5. Copying vs. Sharing for Nondeterminism

When multiple agents share variables and we backtrack, how do we restore state?

**Copying approach**: When creating a choice point, copy the entire state. On backtracking, revert to the copy.

**Sharing approach**: Share data structures, use a trail to record changes, undo changes on backtracking (like Prolog).

AGENTS uses **copying** for simplicity, though this can change computational complexity.

### 6. Memory Management

With concurrent agents, complex data structures, and constraint propagation, memory management is challenging:
- When can data be garbage collected?
- How do we handle cyclic structures?
- How do we maintain reference counts across and-boxes and choice-boxes?

AGENTS uses a **copying garbage collector** to handle this.

## Language Philosophy

AKL embodies several design principles:

### 1. Orthogonality

Features are independent and composable:
- Guards are orthogonal to backtracking
- Ports are orthogonal to shared variables
- Constraints are orthogonal to computation

You can combine features freely.

### 2. Declarative Core with Operational Clarity

AKL has a declarative semantics (what programs mean) but also clear operational semantics (how they execute). This dual view supports both:
- **Reasoning**: Think declaratively about what your program computes
- **Debugging**: Understand operationally how it executes

### 3. Concurrency as Default

Conjunctions can run concurrently by default. The language doesn't force sequentiality. This is forward-thinking: as multicore processors become ubiquitous, and-parallelism is natural.

### 4. Constraint Integration

Constraints are not an afterthought or library—they're integrated into the core execution model with suspension and propagation.

## Summary

AKL is a rich language combining:
- **Logic programming** (Horn clauses, unification, backtracking)
- **Concurrent programming** (and-parallelism, ports, synchronization)
- **Constraint programming** (FD constraints, propagation, search)
- **Committed choice** (guards, don't-care nondeterminism)

This combination creates unique implementation challenges:

- Managing concurrent execution states
- Suspending and waking guards
- Propagating constraints efficiently
- Handling both don't-know and don't-care nondeterminism
- Providing asynchronous communication via ports

The remainder of this book explains how AGENTS tackles these challenges, building a sophisticated abstract machine that brings AKL to life.

---

**Next Chapter**: [Chapter 3: Overall Architecture](chapter-03-overall-architecture.md)
