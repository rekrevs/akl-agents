# Detailed Correction Examples

This document provides specific before/after examples for correcting the systematic errors in the draft chapters.

## Part 1: Syntax Corrections (`:=` vs `:-`)

### Rule 1: Simple Agent Definitions

**Pattern**: Top-level agent definitions must use `:=`

**Before (WRONG)**:
```prolog
fact(0, 1).
fact(N, F) :-
    N > 0,
    N1 is N - 1,
    fact(N1, F1),
    F is N * F1.
```

**After (CORRECT)**:
```
fact(0, 1).
fact(N, F) :=
    N > 0,
    N1 is N - 1,
    fact(N1, F1),
    F is N * F1.
```

### Rule 2: Guarded Choice Definitions

**Pattern**: Top level uses `:=`, inner clauses use `:-` with guards `→`

**Before (WRONG)**:
```prolog
member(X, [X|_]).
member(X, [_|Xs]) :- member(X, Xs).
```

**After (CORRECT - using choice notation)**:
```
member(E, L) :=
  ( L = [E|_] → true
  ; L = [_|L1] → member(E, L1) ).
```

**Alternative (CORRECT - using multi-clause notation)**:
```
member(X, L) :=
  ( L = [X|_] :- true → true
  ; L = [_|Xs] :- true → member(X, Xs) ).
```

Note: The thesis uses both notations. The choice notation is more common.

### Rule 3: Process Definitions with Guards

**Before (WRONG)**:
```prolog
producer(P) :-
    send(data1, P),
    send(data2, P),
    send(done, P).

consumer([H|T]) :-
    process(H),
    consumer(T).
consumer([]).
```

**After (CORRECT)**:
```
producer(P) :=
    send(data1, P),
    send(data2, P),
    send(done, P).

consumer(S) :=
  ( S = [H|T] → process(H), consumer(T)
  ; S = [] → true ).
```

### Rule 4: Recursive Definitions with Conditions

**Before (WRONG)**:
```prolog
append([], Ys, Ys) :- true ? .
append([X|Xs], Ys, [X|Zs]) :- true ? append(Xs, Ys, Zs).
```

**After (CORRECT)**:
```
append(X, Y, Z) :=
  ( X = [] → Z = Y
  ; X = [E|X1] → append(X1, Y, Z1), Z = [E|Z1] ).
```

### Rule 5: Object/Process Definitions

**Before (WRONG)**:
```prolog
bank_account([], _) :- true.
bank_account([withdraw(A)|R], N) :-
    bank_account(R, N - A).
bank_account([deposit(A)|R], N) :-
    bank_account(R, N + A).
bank_account([balance(M)|R], N) :-
    M = N,
    bank_account(R, N).
```

**After (CORRECT)**:
```
bank_account(S, N) :=
  ( S = [] :- true → true
  ; S = [withdraw(A)|R] :- true → bank_account(R, N - A)
  ; S = [deposit(A)|R] :- true → bank_account(R, N + A)
  ; S = [balance(M)|R] :- true → M = N, bank_account(R, N) ).
```

**Alternative (more readable)**:
```
make_bank_account(S) :=
  bank_account(S, 0).

bank_account([], _) :-
  → true.
bank_account([withdraw(A)|R], N) :-
  → bank_account(R, N - A).
bank_account([deposit(A)|R], N) :-
  → bank_account(R, N + A).
bank_account([balance(M)|R], N) :-
  → M = N,
    bank_account(R, N).
```

Note: In this alternative, `make_bank_account` is the **definition** (uses `:=`), and the `bank_account` clauses are **guarded clauses** within an implicit choice (use `:-`).

---

## Part 2: Port Semantics Corrections

### Concept 1: Port Creation

**Before (WRONG)**:
```prolog
% Port is just a variable
setup_communication(P) :- port(P).
```

**After (CORRECT)**:
```
% Port is created with open_port(P, S)
% P is the port (bag), S is the stream (list)
setup_communication(P, S) :=
    open_port(P, S).
```

### Concept 2: Sending Messages

**Before (WRONG - using send/3)**:
```prolog
producer(N, Port) :-
  N > 0 ? send(N, Port, P1),
          N1 is N - 1,
          producer(N1, P1).
producer(0, Port) :- ? Port = [].
```

**After (CORRECT - using send/2 with open_port)**:
```
% Create the producer with a stream output
producer(N, S) :=
    open_port(P, S),
    producer_loop(N, P).

% Send messages to the port
producer_loop(N, P) :=
  ( N > 0 → send(N, P),
            N1 = N - 1,
            producer_loop(N1, P)
  ; N = 0 → true ).
```

Note: When producer terminates, port P becomes garbage, and stream S is automatically closed.

### Concept 3: Receiving Messages

**Before (WRONG - missing the connection)**:
```prolog
consumer([X|Xs]) :- ?
    write(X), nl,
    consumer(Xs).
consumer([]) :- ? true.
```

**After (CORRECT - stream is from open_port)**:
```
% Consumer receives stream from open_port
consumer(S) :=
  ( S = [X|Xs] → write(X), nl, consumer(Xs)
  ; S = [] → true ).
```

### Concept 4: Producer-Consumer Pattern

**Before (WRONG)**:
```prolog
run :-
    producer(5, Stream) & consumer(Stream).

producer(N, Port) :-
    N > 0 ? send(N, Port, P1),
            N1 is N - 1,
            producer(N1, P1).
producer(0, Port) :- ? Port = [].
```

**After (CORRECT)**:
```
% Correct producer-consumer with open_port
run :=
    producer(5, S) & consumer(S).

% Producer creates port and sends messages
producer(N, S) :=
    open_port(P, S),
    producer_loop(N, P).

producer_loop(N, P) :=
  ( N > 0 → send(N, P),
            N1 = N - 1,
            producer_loop(N1, P)
  ; true ).

% Consumer receives from stream
consumer(S) :=
  ( S = [H|T] → write(H), nl, consumer(T)
  ; S = [] → true ).
```

### Concept 5: Multiple Ports (Dataflow Example)

**Before (MISSING from draft)**:

**After (CORRECT - from thesis)**:
```
% Dataflow network: compute sum of squares and cubes
dataflow(N, S) :=
    open_port(P1, S1),
    open_port(P2, S2),
    open_port(P, S),
    gen_sq(N, P1) & gen_cb(N, P2) & add(S1, S2, P).

% Generate squares
gen_sq(N, P) :=
  ( N > 0 → send(N*N, P),
            gen_sq(N-1, P)
  ; true ).

% Generate cubes
gen_cb(N, P) :=
  ( N > 0 → send(N*N*N, P),
            gen_cb(N-1, P)
  ; true ).

% Add streams
add(S1, S2, P) :=
  ( S1 = [N1|T1], S2 = [N2|T2] →
      send(N1+N2, P),
      add(T1, T2, P)
  ; S1 = [], S2 = [] →
      true ).
```

**Key insights**:
1. `open_port(P, S)` creates port P and stream S
2. Messages sent to P appear on S
3. Multiple ports can be created independently
4. When all references to P are gone, S is automatically closed
5. Pattern matching on S (e.g., `S = [H|T]`) receives messages

### Concept 6: Object with Port Interface

**Before (WRONG - using streams directly)**:
```prolog
make_bank_account(S) := bank_account(S, 0).

bank_account([], _) :- true.
bank_account([withdraw(A)|R], N) :- bank_account(R, N - A).
```

**After (CORRECT - using ports)**:
```
% Create bank account with port interface
make_bank_account(P) :=
    open_port(P, S),
    bank_account(S, 0).

% Account processes messages from stream
bank_account(S, N) :=
  ( S = [] → true
  ; S = [withdraw(A)|R] → bank_account(R, N - A)
  ; S = [deposit(A)|R] → bank_account(R, N + A)
  ; S = [balance(M)|R] → M = N, bank_account(R, N) ).

% Client sends to port
client(P) :=
    send(deposit(100), P),
    send(balance(B), P),
    write(B).
```

**Benefits of ports**:
- Multiple clients can send to same port P
- Ports can be embedded in data structures
- Stream S is automatically closed when P becomes garbage
- No need for explicit stream mergers

---

## Part 3: Specific File Corrections

### Chapter 2 (docs/draft/chapter-02-akl-language.md)

**Location**: Lines 153-156

**Before**:
```prolog
fact(N, F) :-
    N > 0,
    N1 is N - 1,
    fact(N1, F1),
```

**After**:
```
fact(N, F) :=
    N > 0,
    N1 is N - 1,
    fact(N1, F1),
```

---

**Location**: Lines 167-168

**Before**:
```prolog
member(X, [X|_]).
member(X, [_|Xs]) :- member(X, Xs).
```

**After**:
```
member(X, L) :=
  ( L = [X|_] → true
  ; L = [_|Xs] → member(X, Xs) ).
```

---

**Location**: Lines 179-181

**Before**:
```prolog
consume(X, [H|T]) :-
  write(H),
  consume(X, T).
```

**After**:
```
consume(X, L) :=
  ( L = [H|T] → write(H), consume(X, T)
  ; L = [] → true ).
```

---

**Location**: Lines 250

**Before**:
```prolog
append([X|Xs], Ys, [X|Zs]) :- append(Xs, Ys, Zs).
```

**After**:
```
append(X, Y, Z) :=
  ( X = [] → Z = Y
  ; X = [E|X1] → append(X1, Y, Z1), Z = [E|Z1] ).
```

---

**Location**: Lines 310-320

**Before**:
```prolog
setup_communication(P) :- port(P).

send_data(Message, Port) :- send(Message, Port).
```

**After**:
```
setup_communication(P, S) :=
    open_port(P, S).

send_data(Message, Port) :=
    send(Message, Port).
```

---

### Chapter 15 (docs/draft/chapter-15-port-communication.md)

**Section to rewrite**: Lines 24-100 (Port Semantics)

**Current text (WRONG)**:
> A **port** in AKL is essentially a **logical variable** that represents the "tail" of a stream. Messages sent to the port are added to the stream as list elements.

**Replacement text (CORRECT)**:

---

## Port Semantics

### The Constraint Model

A **port** in AKL is a **binary constraint** connecting a **bag** (multiset) and a **stream** (list). This constraint states that the bag and stream contain the same messages, in FIFO order.

**Key concepts:**

1. **Port P**: A bag (multiset) of messages
2. **Stream S**: A list of messages
3. **Constraint**: `open_port(P, S)` establishes that P and S contain the same messages

### The open_port Operation

**Syntax**: `open_port(P, S)`

**Semantics**: Creates a port P (bag) and stream S (list), connected by a port constraint.

**Example**:
```
open_port(P, S), send(a, P), send(b, P)

yields

P = 〈a port〉, S = [a, b]
```

After `open_port(P, S)`:
- P is a **port** (you send messages to it)
- S is a **stream** (you pattern-match on it to receive messages)
- Messages sent to P automatically appear on S in FIFO order

### The send Operation

**Syntax**: `send(M, P)`

**Semantics**: Adds message M to port P (which appears on the associated stream).

**Properties**:
- **Asynchronous**: Sender continues immediately
- **Constant delay**: Message appears on stream with constant delay
- **FIFO order**: Messages appear in order sent ("first come first served")

**Example**:
```
producer(N, P) :=
  ( N > 0 → send(N, P), producer(N-1, P)
  ; true ).
```

### Receiving Messages

**Receiving** is done by **pattern matching** on the stream:

```
consumer(S) :=
  ( S = [Msg|Rest] → process(Msg), consumer(Rest)
  ; S = [] → true ).
```

**Suspension**: If stream tail is unbound, consumer suspends until message arrives.

### Automatic Stream Closure

When port P is **garbage collected** (no more references), stream S is **automatically closed**:

- Stream terminates with `[]`
- Consumer detects end of messages
- No explicit "close" operation needed

**Example**:
```
run :=
    producer(5, S) & consumer(S).

producer(N, S) :=
    open_port(P, S),    % Create port P with stream S
    prod_loop(N, P).    % Port P is local to producer

prod_loop(N, P) :=
  ( N > 0 → send(N, P), prod_loop(N-1, P)
  ; true ).             % Producer terminates, P becomes garbage

consumer(S) :=          % Stream S automatically closes
  ( S = [H|T] → write(H), consumer(T)
  ; S = [] → write('done') ).
```

When `prod_loop` terminates:
1. Port P has no more references
2. P becomes garbage
3. Stream S is automatically closed (becomes `[]`)
4. Consumer detects end and terminates

---

**Location**: Lines 349-363 (Producer-Consumer Example)

**Before**:
```prolog
producer(N, Port) :-
  N > 0 ? send(N, Port, P1),
          N1 is N - 1,
          producer(N1, P1).
producer(0, Port) :- ? Port = [].

consumer([X|Xs]) :- ?
  write(X), nl,
  consumer(Xs).
consumer([]) :- ? true.

run :-
  producer(5, Stream) & consumer(Stream).
```

**After**:
```
% Producer-consumer pattern with ports
run :=
    producer(5, S) & consumer(S).

% Producer creates port and sends messages
producer(N, S) :=
    open_port(P, S),
    prod_loop(N, P).

prod_loop(N, P) :=
  ( N > 0 → send(N, P),
            N1 = N - 1,
            prod_loop(N1, P)
  ; true ).

% Consumer receives from stream
consumer(S) :=
  ( S = [X|Xs] → write(X), nl, consumer(Xs)
  ; S = [] → true ).
```

---

## Part 4: Common Patterns Reference

### Pattern 1: Simple Agent

```
agent_name(Args) :=
    constraint1,
    constraint2,
    agent_call(MoreArgs).
```

### Pattern 2: Conditional Agent

```
agent_name(Args) :=
  ( condition1 → body1
  ; condition2 → body2
  ; true → default_body ).
```

### Pattern 3: Recursive Agent

```
process_list(L) :=
  ( L = [H|T] → process(H), process_list(T)
  ; L = [] → true ).
```

### Pattern 4: Port-based Object

```
make_object(P) :=
    open_port(P, S),
    object_loop(S, InitialState).

object_loop(S, State) :=
  ( S = [msg1(Args)|Rest] →
      % handle msg1
      object_loop(Rest, NewState)
  ; S = [msg2(Args)|Rest] →
      % handle msg2
      object_loop(Rest, NewState)
  ; S = [] →
      true ).
```

### Pattern 5: Multi-Port Dataflow

```
network(Inputs, Output) :=
    open_port(P1, S1),
    open_port(P2, S2),
    open_port(Pout, Output),
    stage1(Inputs, P1) &
    stage2(S1, P2) &
    stage3(S2, Pout).
```

### Pattern 6: Parallel Composition

```
parallel_work(Input, Output) :=
    worker1(Input, Out1) &
    worker2(Input, Out2) &
    merger(Out1, Out2, Output).
```

### Pattern 7: Local Variables (Hiding)

```
agent(X, Y) :=
    Z : (
        compute(X, Z),
        compute(Z, Y)
    ).
```

---

## Summary Checklist

When correcting draft chapters:

**Syntax**:
- [ ] Replace `agent_name(...) :-` with `agent_name(...) :=`
- [ ] Keep `:-` only for guarded clauses (with `→`)
- [ ] Use `:` for local variable hiding

**Port Semantics**:
- [ ] Replace "port is a variable" with "port is a bag/stream constraint"
- [ ] Add `open_port(P, S)` to create ports
- [ ] Use `send(M, P)` to send messages (2-argument form)
- [ ] Pattern match on streams to receive: `S = [M|Rest]`
- [ ] Explain automatic closure when port becomes garbage
- [ ] Move `send/3` to implementation details section

**Verification**:
- [ ] All code examples compile
- [ ] Port examples show `open_port`
- [ ] Definitions use `:=`
- [ ] Examples match thesis patterns

---

**Document prepared**: November 7, 2025
**Based on**: Sverker Janson's 1994 AKL thesis (authoritative reference)
