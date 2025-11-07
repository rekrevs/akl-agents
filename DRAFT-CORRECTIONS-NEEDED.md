# Draft Corrections Needed: AKL Syntax and Port Semantics

## Executive Summary

After comparing the draft chapters with Sverker Janson's 1994 thesis, two **critical systematic errors** have been identified throughout the draft:

1. **SYNTAX ERROR**: Draft uses Prolog syntax (`:-`) instead of AKL syntax (`:=`) for definitions
2. **SEMANTIC ERROR**: Draft fundamentally misunderstands port semantics

**Impact**: These errors affect 11+ chapters and require systematic correction throughout the entire draft.

**Priority**: **CRITICAL** - These are not minor issues but fundamental misrepresentations of the AKL language

---

## Issue 1: Incorrect Definition Syntax (`:=` vs `:-`)

### The Problem

**All AKL agent definitions in the draft use PROLOG syntax instead of AKL syntax.**

**Incorrect (Draft)**: Uses `:-` operator
```prolog
fact(N, F) :-
    N > 0,
    N1 is N - 1,
    fact(N1, F1),
    F is N * F1.
```

**Correct (Thesis)**: Uses `:=` operator
```
fact(N, F) :=
    N > 0,
    N1 is N - 1,
    fact(N1, F1),
    F is N * F1.
```

### Formal Specification from Thesis

From **thesis-part-03.md** line 88:

> **Definition syntax**: ⟨name⟩(X₁, …, Xₙ) := ⟨statement⟩.

AKL uses three distinct operators:
- **`:=`** for agent definitions (NOT `:-` which is Prolog)
- **`:-`** for clauses within definitions (guarded choices)
- **`:`** for hiding (local variables)

### Affected Files

**11 chapters contain incorrect `:-` syntax:**

| File | Count | Lines |
|------|-------|-------|
| chapter-02-akl-language.md | 47 | Throughout all examples |
| chapter-17-the-compiler.md | 27 | All compiler examples |
| chapter-15-port-communication.md | 23 | All port examples |
| chapter-16-builtin-predicates.md | 8 | Predicate examples |
| chapter-13-and-boxes-concurrency.md | 4 | Concurrency examples |
| chapter-12-choice-points-backtracking.md | 4 | Backtracking examples |
| chapter-01-introduction.md | 4 | Introduction examples |
| chapter-14-constraint-variables-suspension.md | 3 | CVA examples |
| chapter-05-execution-model.md | 2 | Model examples |
| chapter-11-unification.md | 1 | Unification example |
| chapter-04-pam.md | 1 | PAM example |

**Total**: 124+ incorrect uses of `:-` for definitions

**Note**: Only 5 chapters use `:=` at all, and those uses are for the arithmetic equality operator `=:=`, NOT for definitions!

### Examples from Draft (ALL INCORRECT)

From **chapter-02-akl-language.md**:

```prolog
member(X, [X|_]).                           % LINE 167 - WRONG
member(X, [_|Xs]) :- member(X, Xs).         % LINE 168 - WRONG

append([], Ys, Ys) :- true ? .              % LINE 256 - WRONG
append([X|Xs], Ys, [X|Zs]) :- true ? append(Xs, Ys, Zs).  % LINE 257 - WRONG

producer(P) :-                              % LINE 352 - WRONG
    send(data1, P),
    send(data2, P),
    send(done, P).

consumer(P) :- consume_loop(P).             % LINE 358 - WRONG
```

All of these should use `:=` instead.

### Examples from Thesis (CORRECT)

From **thesis-part-03.md**:

```
plus(X, Y, Z) := Z = X + Y.

append(X, Y, Z) :=
  ( X = [] → Z = Y
  ; X = [E|X1], append(X1, Y, Z1), Z = [E|Z1] ).

member(E, L) :=
  ( L = [E|_] → true
  ; L = [_|L1] → member(E, L1) ).
```

From **thesis-part-15.md**:

```
gen_sq(0, P) :-
  → true.
gen_sq(N, P) :-
  N > 0
  → send(N*N, P),
    gen_sq(N-1, P).

add(P1, P2, P) :=
  open_port(P1, S1),
  open_port(P2, S2),
  add_loop(S1, S2, P).
```

Note: **`:=`** is for the definition head, **`:-`** appears in clauses with guards (indicated by **`→`**).

### Correction Required

**Every definition in the draft must be reviewed and corrected:**

1. **Simple definitions**: Change `agent_name(...) :-` to `agent_name(...) :=`
2. **Guarded definitions**: The top level still uses `:=`, inner clauses use `:-` with `→`
3. **Facts**: Keep as-is (no operator needed)

---

## Issue 2: Fundamental Misunderstanding of Port Semantics

### The Problem

**Draft Chapter 15 completely misunderstands the AKL port model.**

### What the Draft Says (INCORRECT)

From **chapter-15-port-communication.md** lines 24-28:

> A **port** in AKL is essentially a **logical variable** that represents the "tail" of a stream. Messages sent to the port are added to the stream as list elements.

**This is completely wrong.** Ports are NOT logical variables.

### What the Thesis Says (CORRECT)

From **thesis-part-05.md** lines 143-149 (Chapter 3.3.3):

> A **port** is a **binary constraint** on a **bag** (a multiset) of messages and a corresponding **stream** of these messages. It simply states that they contain the same messages, in any order. A bag connected to a stream by a port is usually identified with the port, and is referred to as a port. The **open_port(P, S)** operation relates a bag P to a stream S, and connects them through a port.
>
> The stream S will usually be connected to an object. Instead of using the stream to access the object, we will send messages by adding them to the port. The **send(M, P)** operation sends a message M to a port P. To satisfy the port constraint, a message sent to a port will immediately be added to its associated stream, first come first served.

From **thesis-part-14.md** line 348 (Chapter 7.4):

> The **open_port(P, S)** operation creates a **bag P** and a **stream S**, and connects them through a port. Thus, P becomes a port to S. The **send(M, P)** operation adds a message M to a port P. A message which is sent on a port is added to its associated stream with constant delay. When the port becomes garbage, its associated stream is closed. The **is_port(P)** operation recognises ports.

### Correct Port Model

**Key concepts:**

1. **Port P** = A **bag** (multiset) of messages
2. **Stream S** = A **list** of messages
3. **`open_port(P, S)`** = Binary **constraint** connecting bag P to stream S
4. **`send(M, P)`** = Add message M to bag P (appears on stream S)
5. **Garbage collection** = When P is no longer referenced, S is automatically closed
6. **Order** = Messages appear in FIFO order ("first come first served")

**Ports are NOT variables.** They are **constraint-based communication abstractions**.

### Missing `open_port` Operation

**Critical issue**: Draft chapter 15 does NOT mention `open_port(P, S)` at all!

From search results:
- **Chapter 15**: 0 mentions of `open_port`
- **Thesis Part 15 (Chapter 7)**: 8 mentions of `open_port`

The fundamental operation for creating ports is completely absent from the draft.

### Incorrect Examples in Draft

From **chapter-15-port-communication.md** line 310:

```prolog
setup_communication(P) :- port(P).
```

This is wrong on multiple levels:
1. Uses `:-` instead of `:=`
2. Uses `port(P)` which doesn't exist in AKL
3. Missing the `open_port(P, S)` operation

From **chapter-15-port-communication.md** lines 349-353:

```prolog
producer(N, Port) :-
  N > 0 ? send(N, Port, P1),
          N1 is N - 1,
          producer(N1, P1).
producer(0, Port) :- ? Port = [].
```

This shows `send/3` (send with continuation) which is an **implementation detail**, not the fundamental AKL semantics.

### Correct Examples from Thesis

From **thesis-part-05.md** lines 153-159:

```
open_port(P, S), send(a, P), send(b, P)

yields

P = 〈a port〉, S = [a,b]
```

From **thesis-part-15.md** lines 134-140 (Chapter 7.7.2):

```
dataflow(N, S) :=
  open_port(P1, _),
  open_port(P2, _),
  open_port(P, S),
  gen_sq(N, P1),
  gen_cb(N, P2),
  add(P1, P2, P).
```

This shows the correct usage:
1. **`open_port(P, S)`** creates port P with stream S
2. Multiple ports can be created
3. `send(M, P)` sends to port P (appears on stream S)
4. Stream S can be pattern-matched to receive messages

From **thesis-part-15.md** lines 119-128:

```
add(P1, P2, P) :=
  open_port(P1, S1),
  open_port(P2, S2),
  add_loop(S1, S2, P).

add_loop([], [], _) :-
  → true.
add_loop([N1|S1], [N2|S2], P) :-
  → send(N1+N2, P),
    add_loop(S1, S2, P).
```

This shows:
- `open_port(P1, S1)` connects port P1 to stream S1
- Pattern matching on streams `[N1|S1]` to receive messages
- `send(Result, P)` to send results to output port P

### Correction Required for Chapter 15

**Chapter 15 needs to be substantially rewritten:**

1. **Replace incorrect conceptual model**
   - Remove "port is a variable" description
   - Add correct "port is a bag/stream constraint" model
   - Explain the constraint semantics properly

2. **Add missing `open_port(P, S)` operation**
   - Document `open_port(P, S)` as the PRIMARY operation
   - Explain that P is a port (bag), S is a stream (list)
   - Show that messages sent to P appear on S

3. **Fix all code examples**
   - Change `:-` to `:=` in all definitions
   - Use `open_port(P, S)` to create ports
   - Use `send(M, P)` to send to ports (2-argument form)
   - Pattern match on streams S to receive messages

4. **Demote implementation details**
   - `send/3` is an implementation detail (AGENTS-specific)
   - The fundamental AKL operation is `send/2`
   - CVA implementation is internal detail, not semantics

5. **Add garbage collection semantics**
   - Explain automatic stream closure
   - Show how objects detect termination

---

## Recommended Correction Process

### Phase 1: Chapter 2 (AKL Language)

**File**: `docs/draft/chapter-02-akl-language.md`

**Actions**:
1. Fix all 47 incorrect `:-` to `:=` in definitions
2. Add section on **Definition Syntax** explaining `:=` vs `:-` vs `:`
3. Correct all code examples
4. Add note about difference from Prolog syntax

**Priority**: **CRITICAL** - This is the language reference chapter

### Phase 2: Chapter 15 (Port Communication)

**File**: `docs/draft/chapter-15-port-communication.md`

**Actions**:
1. Rewrite **Port Semantics** section (lines 24-100)
   - Replace "variable" model with "bag/stream constraint" model
   - Add `open_port(P, S)` as primary operation
   - Explain constraint semantics from thesis

2. Reorganize content:
   - **Section 1**: Port semantics (`open_port`, `send`, garbage collection)
   - **Section 2**: Using ports (send/receive patterns)
   - **Section 3**: Implementation (SEND3, CVA, send/3 details)

3. Fix all 23 incorrect `:-` to `:=`

4. Rewrite all examples using correct syntax and semantics

**Priority**: **CRITICAL** - Chapter is fundamentally wrong

### Phase 3: Other Chapters

**Affected chapters**: 1, 4, 5, 11, 12, 13, 14, 16, 17

**Actions**:
1. Fix all `:-` to `:=` in definitions (122 occurrences)
2. Verify port examples use `open_port(P, S)`
3. Check for other syntax inconsistencies

**Priority**: **HIGH** - Must be done for consistency

### Phase 4: Verification

**Actions**:
1. Search for any remaining `function_name(...) :-` patterns
2. Verify all port examples use `open_port`
3. Check hiding syntax uses `:` correctly
4. Compare examples against thesis for correctness

---

## Technical Details from Thesis

### Complete Syntax Specification

From **thesis-part-03.md** (Chapter 2):

**Definitions**:
```
⟨name⟩(X₁, …, Xₙ) := ⟨statement⟩.
```

**Statements**:
```
true                                    % Succeed
⟨constraint⟩                            % Tell constraint
A & B                                   % Parallel composition
A, B                                    % Sequential composition
( C₁ :- G₁ → B₁ ; … ; Cₙ :- Gₙ → Bₙ )  % Choice
⟨name⟩(T₁, …, Tₙ)                      % Agent call
```

**Hiding**:
```
V : ⟨statement⟩                         % Introduce local variable V
```

Note the three distinct operators:
- **`:=`** definition operator
- **`:-`** clause operator (with guard `→`)
- **`:`** hiding operator

### Complete Port Specification

From **thesis-part-14.md** (Chapter 7.4):

**Operations**:
- **`open_port(P, S)`**: Creates bag P and stream S, connects them
- **`send(M, P)`**: Adds message M to bag P
- **`is_port(P)`**: Tests if P is a port

**Constraint semantics**:
- `open_port(P, S)`: All members in bag P are members in stream S (with equal multiplicity)
- `send(M, P)`: M is a member of bag P
- `is_port(P)`: ∃S such that `open_port(P, S)`

**Implementation note** (from thesis-part-14.md line 602):
> In AGENTS, the open_port/2 operation is a specialised open_cc_port/2, which does not accept general continuations, but supports the send/3 operation.

So `send/3` is an AGENTS-specific optimization, not fundamental AKL semantics.

---

## Summary Statistics

### Syntax Errors

- **Files affected**: 11 chapters
- **Total errors**: 124+ incorrect uses of `:-` for definitions
- **Correction**: Change all `agent_name(...) :-` to `agent_name(...) :=`

### Port Semantics Errors

- **Files affected**: Primarily chapter 15, but affects any port discussion
- **Missing operation**: `open_port(P, S)` not documented
- **Incorrect model**: "Ports are variables" (should be "ports are bag/stream constraints")
- **Correction**: Rewrite chapter 15 port semantics section from thesis

### Impact Assessment

**Severity**: **CRITICAL**

These are not typos or minor errors. They are:
1. **Systematic syntax errors** that make all code examples incorrect
2. **Fundamental semantic misunderstandings** of a core language feature

**User Impact**: Anyone learning AKL from this draft will learn incorrect syntax and incorrect port semantics.

**Recommendation**: These errors should be corrected before any further work on the draft.

---

## References

### Thesis Files

- **thesis-part-01.md**: Title page, table of contents
- **thesis-part-02.md**: Preface, Chapter 1 (Introduction)
- **thesis-part-03.md**: Chapter 2 (Language Overview) - **Syntax reference**
- **thesis-part-05.md**: Chapter 3 (Programming Paradigms) - **Port introduction**
- **thesis-part-14.md**: Chapter 7 (Ports for Objects) - **Port formal semantics**
- **thesis-part-15.md**: Chapter 7 continued - **Port examples**

### Draft Files Requiring Correction

**Critical priority**:
- `docs/draft/chapter-02-akl-language.md` (47 syntax errors, language reference)
- `docs/draft/chapter-15-port-communication.md` (23 syntax errors, wrong semantics)

**High priority**:
- `docs/draft/chapter-17-the-compiler.md` (27 syntax errors)
- `docs/draft/chapter-16-builtin-predicates.md` (8 syntax errors)
- `docs/draft/chapter-13-and-boxes-concurrency.md` (4 syntax errors)
- `docs/draft/chapter-12-choice-points-backtracking.md` (4 syntax errors)
- `docs/draft/chapter-01-introduction.md` (4 syntax errors)
- `docs/draft/chapter-14-constraint-variables-suspension.md` (3 syntax errors)
- `docs/draft/chapter-05-execution-model.md` (2 syntax errors)
- `docs/draft/chapter-11-unification.md` (1 syntax error)
- `docs/draft/chapter-04-pam.md` (1 syntax error)

---

**Document prepared**: November 7, 2025
**Branch**: `claude/thesis-review-011CUtkv2EJhgbVJtVJsFh6o`
**Based on**: Comparison of draft chapters vs. Sverker Janson's 1994 AKL thesis
