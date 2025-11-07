# Chapter 15: Port Communication

## Introduction

**Ports** are AKL's mechanism for **asynchronous message passing** between concurrent agents. Unlike traditional logic programming where communication happens through shared variables and unification, ports provide a **stream-based** communication model inspired by concurrent logic languages like Concurrent Prolog and GHC (Guarded Horn Clauses).

Ports enable:

1. **Asynchronous communication**: Sender doesn't wait for receiver
2. **Decoupling**: Producer and consumer are independent
3. **Stream processing**: Messages form a stream (list)
4. **Concurrent programming**: Multiple agents communicate via ports

This chapter explores:

- **Port semantics** and the stream model
- **SEND3 instruction** implementation
- **Port constraints** and CVA integration
- **Send and receive built-ins**
- **Producer-consumer examples**
- **Implementation details** and performance

Ports distinguish AKL from Prolog and make it suitable for concurrent, reactive programming.

## Port Semantics

### The Constraint Model

A **port** in AKL is a **binary constraint** connecting a **bag** (multiset) and a **stream** (list). This constraint states that the bag and stream contain the same messages, delivered in FIFO order.

**Key concepts**:

1. **Port P**: A **bag** (multiset) of messages
2. **Stream S**: A **list** of messages
3. **Constraint**: `open_port(P, S)` establishes that P and S contain the same messages

From Janson's thesis (Chapter 7.4):

> "A port is a binary constraint on a bag (a multiset) of messages and a corresponding stream of these messages. It simply states that they contain the same messages, in any order."

**Ports are NOT variables** - they are constraint-based communication abstractions.

### The open_port Operation

**Syntax**: `open_port(P, S)`

**Semantics**: Creates a port P (bag) and stream S (list), connected by a port constraint.

**Example from thesis**:

```
open_port(P, S), send(a, P), send(b, P)

yields

P = 〈a port〉, S = [a, b]
```

After `open_port(P, S)`:
- **P** is a port (you send messages to it)
- **S** is a stream (you pattern-match on it to receive messages)
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

Multiple sends to the same port P are allowed because P is a bag, not a variable.

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

### Implementation Note: send/3

In AGENTS, there is also a `send/3` operation: `send(Message, InPort, OutPort)`.

**This is an AGENTS-specific optimization**, not fundamental AKL semantics. From Janson's thesis (Chapter 7.4):

> "In AGENTS, the open_port/2 operation is a specialised open_cc_port/2, which does not accept general continuations, but supports the send/3 operation."

The `send/3` operation provides a "continuation" style for efficiency in certain implementation patterns. However, the fundamental AKL operation is `send/2` with `open_port/2`.

**send/3 usage**:

```
send(Message, InPort, OutPort)
```

This binds `InPort = [Message|OutPort]`, providing the new port tail. This is useful for implementation but not required for understanding port semantics.

All examples in this chapter use the fundamental `send/2` operation unless otherwise noted.

## The SEND3 Instruction

The `SEND3` instruction implements the atomic send operation at the bytecode level.

### Instruction Format

**From opcodes.h**:

```c
Item(SEND3)
```

**Arguments**: None (uses registers X0, X1, X2)

**Register convention**:

- **X0**: Message to send
- **X1**: Input port
- **X2**: Output port (unbound variable, will be unified with new tail)

### Implementation

**From engine.c:2674-2731**:

```c
CaseLabel(SEND3):
{
  Term inport, message, outport;
  Reference ref;
  bool res;
  bool (*sfunc)();

  Instr_None(SEND3);

  SaveRegisters;

  // Get the input port
  Deref(inport, X(1));

  // Check if port is a CVA (constrained variable)
  if (!IsCvaTerm(inport))
    goto sendothercase;

  ref = Ref(inport);
  DerefGvaEnv(RefGva(ref));

  // Get the send method from CVA method table
  sfunc = RefCvaMethod(ref)->send;

  // Check if port is local and has send method
  if(IsLocalGVA(ref, andb) && (sfunc != NULL)) {

    Deref(message, X(0));

    // Call the send method
    res = sfunc(message, inport, exs);

    if(res == TRUE) {
      // Success: unify output port with new tail
      Deref(outport, X(2));
      UNIFY(outport, inport, res);
    }

    if(res == TRUE) {
      RestoreRegisters;
      FetchOpCode(0);
      NextOp();
    } else if(res == FALSE) {
      RestoreRegisters;
      goto fail;
    } else {  // res == SUSPEND
      RestoreRegisters;
      goto choicehandler;
    }
  }

sendothercase:
  // Fall back to send/3 built-in
  RestoreRegisters;
  def = send3_def;
  arity = 3;
  goto call_definition;
}
```

**Key steps:**

1. **Check port type**: Must be CVA (constrained variable with port constraint)
2. **Get send method**: Extract from CVA method table
3. **Call send method**: Invoke port-specific send operation
4. **Handle result**:
   - **TRUE**: Success, unify output port
   - **FALSE**: Failure
   - **SUSPEND**: Port full or blocked, suspend agent

### Port as CVA

Ports are implemented as **CVA** (Constrained Variables) with a special **port constraint**.

**Port constraint structure** (from port.c:28-41):

```c
typedef struct port_constr {
  constrtable   *method;
  constraint    *next;
} port_constr;

static constrtable port_constr_methods = {
  install_port_constr,
  reinstall_port_constr,
  deinstall_port_constr,
  promote_port_constr,
  print_port_constr,
  copy_port_constr,
  gc_port_constr,
};
```

The port constraint manages the port's state and stream.

## Built-in Predicates

### send/3 - Send with Continuation

**Signature**: `send(Message, InPort, OutPort)`

**Implementation** (from port.c:496-550, simplified):

```c
bool akl_send_3(Arg)
     Argdecl;
{
  Term theport, message, outstream;

  Deref(message, A(0));      // Message to send
  Deref(theport, A(1));      // Input port

  if(IsVar(theport)) {
    Reference ref = Ref(theport);

    if(VarIsGVA(ref)) {
      // Port is a CVA: call its send method
      bool (*sfunc)() = RefCvaMethod(ref)->send;

      if(sfunc != NULL) {
        bool result = sfunc(message, theport, exs);
        if(result == TRUE) {
          // Success: unify output port
          return unify(A(2), theport, exs->andb, exs);
        }
        return result;
      }
    }

    // Port is a simple variable: instantiate to [Message|NewPort]
    Term cons, newport;
    NewList(cons);
    LstCar(Lst(cons)) = message;

    MakeBigVar(Ref(theport));
    newport = TagGva(RefGva(ref));
    LstCdr(Lst(cons)) = newport;

    // Bind port to cons cell
    if(!unify(theport, cons, exs->andb, exs))
      return FALSE;

    // Unify output port with new tail
    return unify(A(2), newport, exs->andb, exs);
  }

  return FALSE;
}
```

**Algorithm:**

1. **Port is CVA**: Call send method (may suspend)
2. **Port is variable**: Create `[Message|NewPort]`, bind port, return NewPort
3. **Port is bound**: Fail (port closed)

### send/2 - Simple Send

**Signature**: `send(Message, Port)`

**Implementation** (from port.c:439-494):

```c
bool akl_send_2(Arg)
     Argdecl;
{
  Term theport, message;

  Deref(message, A(0));
  Deref(theport, A(1));

  if(IsVar(theport)) {
    Reference ref = Ref(theport);

    if(VarIsUVA(ref)) {
      // Upgrade to CVA if needed
      MakeBigVar(ref);
    }

    if(VarIsGVA(ref)) {
      // Port is variable: create [Message|NewPort]
      Term cons;
      NewList(cons);
      LstCar(Lst(cons)) = message;

      // Get current "insertion point" (tail of stream)
      Term point = RefTerm(ref);

      // Create new tail
      Term newtail;
      MakeBigVar(Ref(point));
      newtail = TagGva(RefGva(Ref(point)));
      LstCdr(Lst(cons)) = newtail;

      // Unify insertion point with new cons cell
      return unify(point, cons, exs->andb, exs);

    } else {
      // CVA: call send method
      bool (*sfunc)() = RefCvaMethod(Ref(theport))->send;

      if(sfunc != NULL)
        return sfunc(message, theport, exs);
      else
        return FALSE;
    }
  }

  return FALSE;
}
```

**Difference from send/3**: No output port—sender doesn't get continuation.

## Producer-Consumer Example

### Simple Producer-Consumer

**AKL code using correct port semantics**:

```
% Run both concurrently
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
  ; true ).                        % Port P becomes garbage, S closes

% Consumer processes messages from stream
consumer(S) :=
  ( S = [X|Xs] → write(X), nl, consumer(Xs)
  ; S = [] → true ).               % Stream closed
```

**Execution trace**:

1. **Start**: `run` creates two concurrent agents:
   - `producer(5, S)`
   - `consumer(S)`

2. **Producer creates port**: `open_port(P, S)`
   - Port P and stream S are created and connected
   - Producer continues with `prod_loop(5, P)`

3. **Producer sends**: `send(5, P)`
   - Message 5 is added to port P
   - Appears on stream S: `S = [5|S1]` (where S1 is unbound tail)

4. **Consumer receives**: Match `S` with `[X|Xs]`
   - `S` is bound to `[5|S1]`
   - `X = 5`, `Xs = S1`
   - Print 5, recurse on `S1`

5. **Consumer suspends**: Match `S1` with `[X|Xs]`
   - `S1` is unbound!
   - SUSPEND_FLAT suspends consumer

6. **Producer sends**: `send(4, P)`
   - Message 4 is added to port P
   - Appears on stream: `S1 = [4|S2]`
   - **WakeAll** wakes consumer

7. **Consumer resumes**: Match `S1` (now `[4|S2]`) with `[X|Xs]`
   - `X = 4`, `Xs = S2`
   - Print 4, recurse

8. **Repeat** until producer terminates, port P becomes garbage, stream S closes with `[]`

**Key insight**: Suspension and waking synchronize producer and consumer automatically. Port P and stream S are connected by the port constraint.

### Buffered Port

Ports can implement **buffering** to decouple producer and consumer speeds:

```prolog
% Buffered port with capacity N
buffer(N, In, Out) :-
  N > 0 ? In = [X|In1],
          Out = [X|Out1],
          buffer(N, In1, Out1).
buffer(0, In, Out) :- ?
  Out = [X|Out1],
  In = [X|In1],
  buffer(0, In1, Out1).

% Producer sends to buffered input
producer(N, Port) :- ...  % As before

% Consumer reads from buffered output
consumer(Stream) :- ...   % As before

% Run with buffer
run :-
  buffer(10, BufIn, BufOut) &
  producer(100, BufIn) &
  consumer(BufOut).
```

**Effect**: Buffer allows producer to run ahead of consumer by up to 10 messages.

## Stream Transformation

Ports enable elegant **stream transformation** pipelines.

### Filter

```prolog
% Filter stream: keep elements satisfying predicate P
filter(P, [X|Xs], [Y|Ys]) :-
  call(P, X) ?
    Y = X,
    filter(P, Xs, Ys).
filter(P, [X|Xs], Ys) :-
  not(call(P, X)) ?
    filter(P, Xs, Ys).
filter(_, [], []) :- ? true.

% Usage
even(X) :- X mod 2 =:= 0.

run :-
  producer(10, S1) &
  filter(even, S1, S2) &
  consumer(S2).
```

**Result**: Consumer receives only even numbers.

### Map

```prolog
% Map function F over stream
map(F, [X|Xs], [Y|Ys]) :- ?
  call(F, X, Y),
  map(F, Xs, Ys).
map(_, [], []) :- ? true.

% Usage
double(X, Y) :- Y is X * 2.

run :-
  producer(5, S1) &
  map(double, S1, S2) &
  consumer(S2).
```

**Result**: Consumer receives doubled numbers (2, 4, 6, 8, 10).

### Pipeline

Combining transformations:

```prolog
run :-
  producer(20, S0) &
  filter(prime, S0, S1) &
  map(square, S1, S2) &
  filter(less_than(100), S2, S3) &
  consumer(S3).
```

**Effect**: Multi-stage concurrent pipeline processing primes squared, keeping results < 100.

## Implementation Details

### Port Constraint Integration

Ports integrate with the constraint system through CVA:

1. **Create port**: Allocate CVA with port constraint
2. **Send method**: Custom send operation
3. **Suspension**: Port can suspend sender (e.g., bounded buffers)
4. **GC**: Port constraints participate in garbage collection

**Creating a port constraint** (port.c:98-108):

```c
constraint *make_port_constraint(exstate *exs)
{
  port_constr *constr;
  NEW(constr, port_constr);
  constr->method = &port_constr_methods;

  LinkConstr(exs->andb, (constraint*)constr);

  return (constraint*)constr;
}
```

### Garbage Collection

Ports have special GC handling because they reference external state (streams).

**Port GC method** (port.c:86-93):

```c
constraint *gc_port_constr(old, gcst)
     port_constr *old;
     gcstatep gcst;
{
  port_constr *new;
  NEW(new, port_constr);
  return (constraint*)new;
}
```

**Stream closing** (port.c:155-173):

```c
void close_port_stream(s)
    Term *s;
{
  register Term x = *s;

  // Follow references
  while (IsREF(x)) {
    if (IsCopied(Ref(x))) {
      x = TagRef(Forw(Ref(x)));
      break;
    } else if (IsUnBoundRef(Ref(x))) {
      break;
    }
    x = RefTerm(Ref(x));
  }

  // If stream is bound to non-NIL, force failure
  if (!(IsREF(x) && IsUnBoundRef(Ref(x)) || Eq(x, NIL)))
    *s = MakeSmallNum(0);    // Force failure sentinel

  return;
}
```

This ensures ports are properly closed during GC and promotion.

### Performance

**Send operation cost**:

1. **Allocate cons cell**: ~10 cycles (bump-pointer)
2. **Unify port with cons**: ~20 cycles (fast path)
3. **Wake suspended agents**: ~50 cycles if any waiting
4. **Total**: ~30-80 cycles per send

**Comparison**:

- **Shared variable**: 0 cycles (just unification)
- **Port send**: ~50 cycles average
- **OS message passing**: 1000+ cycles (system call)

**Tradeoff**: Ports are slower than shared variables but enable **asynchronous** communication and **decoupling**.

## Advanced Features

### Bidirectional Ports

Ports can be used for bidirectional communication:

```prolog
% Server receives requests, sends replies
server(InPort, OutPort) :-
  InPort = [req(Data, ReplyPort)|In1] ?
    process(Data, Result),
    send(Result, ReplyPort, _),
    server(In1, OutPort).

% Client sends request, waits for reply
client(Request, Result, ServerIn) :-
  send(req(Request, ReplyPort), ServerIn, _),
  ReplyPort = [Result|_] ? true.
```

**Effect**: Request-reply pattern using ports.

### Multiplexing

Multiple producers can send to same port:

```prolog
run :-
  producer1(Stream) &
  producer2(Stream) &
  consumer(Stream).
```

**Semantics**: Messages **interleave** non-deterministically.

**Challenge**: Ensuring correct order may require additional coordination.

### Port Arrays

Ports can be organized into arrays for structured communication:

```prolog
% Worker pool
workers(N, In, Out) :-
  N > 0 ?
    worker(In, W),
    workers(N-1, W, Out).
workers(0, Out, Out) :- ? true.

worker([Task|Rest], Out) :- ?
  process(Task, Result),
  Out = [Result|Out1],
  worker(Rest, Out1).
```

**Effect**: Pipeline of workers processing tasks.

## Comparison with Other Models

### Prolog Streams

**Prolog**: Streams are files or I/O devices

**AKL**: Streams are logical lists with port variables

**Advantage**: AKL streams are first-class, composable, and suspendable.

### Concurrent Prolog

**Concurrent Prolog**: Read-only variables, stream communication

**AKL**: Ports + guards + committed choice

**Difference**: AKL's guards provide more flexible commitment control.

### Erlang Mailboxes

**Erlang**: Actor model, asynchronous mailboxes

**AKL**: Ports are logical streams

**Similarity**: Both provide asynchronous message passing.

**Difference**: AKL integrates with unification and logic programming.

## Summary

Ports are AKL's mechanism for concurrent, asynchronous communication:

1. **Stream model**: Ports represent tails of logical streams
   - Messages added as list elements
   - Progressive instantiation builds stream

2. **Send operations**:
   - **send/2**: Simple send (no continuation)
   - **send/3**: Send with continuation (standard)
   - **SEND3**: Bytecode instruction for atomic send

3. **Implementation**:
   - Ports as CVA (constrained variables)
   - Port constraint integrates with constraint system
   - Send method handles operation
   - GC support for stream closing

4. **Synchronization**:
   - Receiver suspends on unbound stream tail
   - Sender wakes receiver when message sent
   - Automatic synchronization via suspension/wake

5. **Applications**:
   - Producer-consumer patterns
   - Stream transformation (filter, map)
   - Concurrent pipelines
   - Bidirectional communication

6. **Performance**:
   - ~50 cycles per send (average)
   - Faster than OS message passing
   - Slower than shared variables
   - Good tradeoff for asynchronous communication

**Key advantages**:

- **Asynchronous**: Sender doesn't block
- **Decoupled**: Producer/consumer independent
- **Composable**: Streams are first-class values
- **Suspendable**: Automatic synchronization
- **Logical**: Integrates with unification

**Compared to**:

- **Shared variables**: Ports enable asynchrony
- **Channels**: Ports are logical (backtrackable)
- **Mailboxes**: Ports are streams (ordered)

Ports, combined with:
- **Guards** (Chapter 5)
- **Suspension** (Chapter 14)
- **Concurrency** (Chapter 13)

Enable AKL to express concurrent, reactive, stream-processing programs elegantly and efficiently.

**Example applications**:

- **Pipeline processing**: Multi-stage data transformation
- **Event handling**: React to messages asynchronously
- **Parallel algorithms**: Divide-and-conquer with communication
- **Simulation**: Concurrent processes exchanging messages

Ports are a distinguishing feature that sets AKL apart from traditional Prolog and makes it suitable for **concurrent constraint programming**.

---

**Key source files:**

- `emulator/port.c:439-550` - send/2 and send/3 built-ins
- `emulator/engine.c:2674-2731` - SEND3 instruction
- `emulator/port.c:28-108` - Port constraint implementation
- `emulator/port.c:155-200` - GC and stream closing
- `demos/` - Various examples using ports
