# Thesis Part 15

## PORTS FOR OBJECTS (continued)

```
cellproc([], _) :-
  → true.
cellproc([read(V)|S], N) :-
  → V = N,
    cellproc(S, N).
cellproc([write(V)|S], _) :-
  → cellproc(S, V).
```

A memory is an array of cells.

```
memory(N, M) :=
  array_of_cells(0, N, M).

array_of_cells(N, N, M) :-
  → M = [].
array_of_cells(I, N, M) :-
  I < N
  → cell(P),
    M = [P|M1],
    array_of_cells(I+1, N, M1).
```

Processors are processes with a local state and a program counter, taking instructions from a program and executing them on a memory. Apart from performing reads and writes, each instruction may increment the program counter (to continue with the next instruction) or jump (to some other instruction). A processor program is an array of instructions.

```
processor(IP, Local, Program, Memory) :=
  execute(IP, Local, Program, Memory).

execute(IP, Local, Program, Memory) :-
  → lookup(Program, IP, Instruction),
    Local1 = Local,
    interpret(Instruction, IP, Local, IP1, Local1, Memory),
    execute(IP1, Local1, Program, Memory).
```

An instruction interpreter has to interpret a given instruction set. As an example, let us say that we have load, store, jump, and halt instructions.

```
interpret(load(A, L), IP, Local, IP1, Local1, Memory) :-
  → lookup(Memory, A, Cell),
    send(read(V), Cell),
    update(Local, L, V, Local1),
    IP1 = IP+1.
interpret(store(L, A), IP, Local, IP1, Local1, Memory) :-
  → lookup(Memory, A, Cell),
    lookup(Local, L, V),
    send(write(V), Cell),
    Local1 = Local,
    IP1 = IP+1.
interpret(jump(A), _, Local, IP1, Local1, Memory) :-
  → Local1 = Local,
    IP1 = A.
interpret(halt, _, Local, IP1, Local1, Memory) :-
  → true.
```

An EREW-PRAM is a multiprocessor with *N* processors sharing a common memory of size *M*, where only one processor at a time may read or write each memory cell. This is easily simulated using the above definitions by creating a memory and *N* processors.

```
pram(N, M, Program) :=
  memory(M, Memory),
  processors(0, N, Program, Memory).

processors(N, N, _, _) :-
  → true.
processors(I, N, Program, Memory) :-
  I < N
  → processor(0, initial_local_state, Program, Memory),
    processors(I+1, N, Program, Memory).
```

Note that each processor may send read and write requests on cell ports in the shared memory, which makes the shared memory behave like a multiport [Saraswat 1989; Saraswat, Jagadeesan, and Gupta 1994], an object with several points for inserting messages in the message queue. A one-element queue *between the processors and each cell* allows one message to be added to the port while the cell is processing the previous message. This reduces the latency compared to if the message would have had to wait for the cell to process the previous message before it could start being sent on the port.

The definition of a PRAM given in this section is a conceptual PRAM. We do not expect all abstract parallelism in the definition to be realised by an implementation. Assume that the implementation only provides a given number of physical processors, and that other processes will reside in memory or on secondary storage, migrating to processors for execution when necessary. For the kinds of applications we consider, many of the processes can be eliminated by program transformation using partial evaluation or related techniques.

### 7.7 APPLICATIONS

In this section we will discuss a few diverse applications of ports.

#### 7.7.1 The Actor Model

An Actor is an object with a mail address [Agha 1986]. An Actor may receive messages, create other Actors, and send messages to Actors for which it knows the mail address. A message is guaranteed to reach its destination, but there is no guarantee on delivery time, and the order of messages may be scrambled. When an Actor has processed a message, it may become a new Actor with a new behaviour.

Ports have constant message sending delay. If one sends a message to a port, it is guaranteed to reach its destination. No assumption is made about when it will arrive. The order of messages on a stream associated with a port is nondeterministic. Summarising these observations, ports satisfy the requirements placed on Actor mail addresses.

The possibility to "become" was discussed in the chapter on language overview. An object may replace itself with an object of a different kind. With ports, one may also (conceptually) "become" a new object with a new mail address, simply by opening a new port for the stream of incoming messages, and perhaps forwarding this new mail address in a message to some interested party.

#### 7.7.2 Dataflow

Dataflow is a model of parallel computation which has been proposed as the basis for high-level programming languages (see, e.g., [Ackerman 1982; Arvind and Nikhil 1990]). In its pure form, a dataflow computation consists of actors forming a directed acyclic graph. The directed arcs in the graph are queues of tokens, and the actors are primitive functions. When all input queues of an actor have nonempty contents, the actor may "fire", reading and removing tokens from its input queues, and appending computed results to its output queues.

Consider the following example: computing in parallel the sum of the first *n* squares and the first *n* cubes. Assume we have generators for the first *n* squares and the first *n* cubes respectively, and an adder. To each we add a port as an output queue.

```
gen_sq(0, P) :-
  → true.
gen_sq(N, P) :-
  N > 0
  → send(N*N, P),
    gen_sq(N-1, P).

gen_cb(0, P) :-
  → true.
gen_cb(N, P) :-
  N > 0
  → send(N*N*N, P),
    gen_cb(N-1, P).
```

The adder takes two input queues. When there is an element in each, it is activated and computes a sum, which is sent on the output queue.

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

We then compose a dataflow network, and collect results on a stream.

```
dataflow(N, S) :=
  open_port(P1, _),
  open_port(P2, _),
  open_port(P, S),
  gen_sq(N, P1),
  gen_cb(N, P2),
  add(P1, P2, P).
```

This dataflow network computes as expected. When the generators have run to completion, the objects that consume messages from the ports will be notified automatically.

Dataflow networks are used primarily for data-parallel numerical computing and for signal processing, where massive amounts of data are to be processed. This simple example gives an indication of how such computations could be expressed in AKL. For practical purposes, arrays or streams of numbers are better represented as Prolog difference-lists, which allow more efficient representation.

A similar technique can be used to emulate the parallel constructs in SISAL [McGraw et al. 1985], which are also based on automatic detection of termination of streams of values.

#### 7.7.3 Distributed Termination

Ports provide automatic termination for simple tree-structured process networks. For arbitrary networks, there are several well-known algorithms for distributed termination detection that may be used, e.g., [Shapiro 1986a]. It should be noted that the introduction of ports do not exclude the use of short-circuiting.

#### 7.7.4 Linda

Linda is a parallel programming model and language based on a shared data structure called a *tuple space* [Carriero and Gelernter 1989]. Processes may insert tuples into tuple space, and extract matching tuples from it. Tuple space therefore acts as both a communication medium and an associative memory. Matching of tuples is based on types and values.

To implement Linda tuple space, we create a server process with a port. As a first approximation, the server could store the tuples internally in a list. For better performance, the server could keep tuples in a hash-table indexed by the types of tuple components, or in some other data structure suitable for fast associative lookup. Messages sent to the server are of the following kinds: requests to insert, test for presence of, or extract tuples from tuple space, where the latter two may be either blocking or nonblocking. Blocking operations contain incomplete messages [Shapiro 1989], while nonblocking operations do not.

```
linda_server(P) :=
  open_port(P, S),
  linda(S, []).

linda([], _) :-
  → true.
linda([in(T)|S], TS) :-
  → linda(S, [T|TS]).
linda([out(T, Reply)|S], TS) :-
  member_del(T, TS, TS1)
  → Reply = yes,
    linda(S, TS1).
linda([out(T, Reply)|S], TS) :-
  → Reply = no,
    linda(S, TS).
```

and so on. In practice, it may be useful to add the possibility to close tuple space.

The ease with which Linda can be expressed using ports indicates that Linda is really an object-oriented model of parallel programming.

#### 7.7.5 Mutable Arrays

Shapiro [1989] discusses incomplete messages and reply messages as a means to implement shared "passive" read-write objects, as exemplified by an array. The following code defines a one-dimensional array *A* of size *N*, with operations to read *A*[*I*] and assign *A*[*I*]:=*X*.

For simplicity, the elements are assumed to be initially unbound. The array is represented by an object with a port *P*, which uses a vector *V* to hold the array components. A vector *V* can be thought of as a Prolog term with *N* arguments, the elements of the vector, and a function (built-in predicate) arg(*I*, *V*, *E*) that gets the *I*:th element *E* of the vector *V*, and a similar predicate for updating vectors.

```
array(N, P) :=
  open_port(P, S),
  make_vector(N, V),
  array_server(S, V).

array_server([], _) :-
  → true.
array_server([A[I] := X|S], V) :-
  → update_arg(I, V, X),
    array_server(S, V).
array_server([A[I] =? X|S], V) :-
  → arg(I, V, X),
    array_server(S, V).
```

Note that no reply is necessary for assignments to the array, thus they are not incomplete messages. Reads, on the other hand, are incomplete messages. When received by the array server, the element is extracted from the vector, and bound to the variable in the message. The result is available to the sender without further message passing.

It is possible to implement this array entirely in the AGENTS run-time system, with further gains in efficiency. Arrays are heavily used data structures, which motivates such built-in support.

#### 7.7.6 Atomic Actions

Ports can be used for another useful purpose: implementing atomic actions, such as test-and-set and compare-and-swap, which are necessary building blocks when programming monitors, semaphores, and other synchronisation constructs [Andrews 1991]. Using messages with reply values, an object will automatically execute one request at a time. For instance, the compare_and_swap object

```
cas(P) :=
  open_port(P, S),
  cas_server(S, null).

cas_server([], _) :-
  → true.
cas_server([cas(Old1, New, Old2)|S], Old) :-
  → ( Old1 = Old
    → cas_server(S, New),
      Old2 = Old
    ; cas_server(S, Old),
      Old2 = Old ).
```

performs a compare-and-swap on an internal state, initially null.

This can be used to implement a parallel version of the ground representation [Colmerauer 1982; 1984], which is a space-efficient way to represent large ground terms with shared subterms. A ground term can be represented (in part) by an unbound logical variable which will be ground when it is dereferenced. This allows terms to be constructed incrementally, where producers and consumers of partially constructed terms work in parallel. For example, when representing a list pair, the head may be under construction by a producer, and at the same time, some consumer may wish to make a copy of the list pair, before its head has been completed. To ensure that only one copy of the head is created, it is necessary to ensure that only one process starts constructing it. Using a compare-and-swap object, this can be controlled. The following function creates a copy of a term in the ground representation, and a copy of that representation at the same time.

```
copy_ground(T, C, G) :=
  deref(T, DT),
  ( simple(DT)
  → C = DT,
    G = DT
  ; compound(DT)
  → DT =.. [F|Args],
    maplist(copy_ground, Args, CArgs, GArgs),
    C =.. [F|CArgs],
    G =.. [F|GArgs]
  ; var(DT)
  → arg(DT, 1, R),
    send(cas(null, started, Old), R),
    ( Old = null
    → deref(DT, DT1),
      copy_ground(DT1, C, G1),
      arg(DT, 2, G2),
      G2 = G1,
      G = G2
    ; Old = started
    → arg(DT, 2, G),
      copy_ground(G, C, G)
    )).
```

Assume *T* is a term in the ground representation to be copied. If it is a simple term (a number or symbol), it is returned both as an ordinary copy *C*, and as a term in the ground representation *G*. If it is a compound term, it is taken apart into its functor and argument list, and the same operation is performed (recursively) on each argument. The result lists are combined using the functor to construct both an ordinary copy *C*, and a copy in the ground representation *G*.

If *T* is still an unbound variable, it is by convention a two-argument term, in which the first argument is a port to a compare-and-swap object *R*, and the second argument is a ground representation of the dereferenced *T*. Two cases can occur. First, if the compare-and-swap object is in its initial state, it is updated to a *started* state, ensuring that other copy operations will not dereference the variable again. After it has been dereferenced, the copying is redone from the dereferenced term *DT1*. The ground representation copy is bound to the second argument of the variable, so that it can be shared by other copying operations. Second, if the compare-and-swap object is in the *started* state, somebody else has started the job of dereferencing. The second argument of the variable is then used as the ground representation of the term to be copied.

In this example, the representation used for variables is somewhat over-simplified. For a more realistic example, see [Janson and Montelius 1992].

### 7.8 RELATED WORK

The requirements in Section 7.2 were motivated by Janson [1991; 1992], which also describes the initial development of ports. Chapter 7 is partially based on, and develops the ideas in, Janson and Haridi [1991], which introduces ports, and provides examples of their use.

The multiway merger using bagof and feedback presented in this chapter can be found in Haridi and Janson [1990].

Smolka, Henz, and Würtz [1993; 1995] use port-like objects as a starting point for a calculus for higher-order concurrent constraint programming with first-class abstractions. Their calculus can be translated back into AKL if desired. This goes to show that a kernel language with a very small number of primitive notions can provide a basis for a variety of linguistic extensions.

Bahgat [1992] describes an approach to modules in a concurrent logic programming language using assumptions (or hypotheses), which are similar to ports in many respects. However, the motivation for and application of assumptions are different from ours.

Saraswat [1989] and Saraswat, Jagadeesan, and Gupta [1994] introduce ask and tell primitives and multiple tell ("multiport") as linguistic primitives in cc(FD). Our results suggest that such a treatment is not necessary in AKL.

### 7.9 SUMMARY

We have discussed requirements on communication abstractions for object-oriented programming in AKL. We have shown that these abstractions should:

• Preserve the monotonicity of the constraint store.

• Have the constant delay property.

• Allow garbage collection of consumed message holders.

• Allow automatic garbage collection of objects.

• Allow multiple messages to be sent to the same object.

We have shown that streams, as communication media, do not fully satisfy these requirements, and we have reviewed a number of previously suggested solutions, including mutual references, channels, and bags, and found them wanting.

We have proposed ports as a solution. Ports are bags of messages connected to streams of these messages. We have shown that, given ports,

• no mergers have to be created,

• several messages can be sent on the same port,

• message sending delay is constant,

• senders do not prevent garbage collection,

• ports are closed automatically,

• the transparent forwarding problem is solved,

• messages can be sequenced.

We have informally discussed the semantics of ports as a special form of constraints, and formalised the operational semantics of ports by reduction rules. We have also discussed implementation techniques for ports, and for objects based on ports.

We have shown how continuation-based synchronisation idioms can be provided with very low overhead. We have argued that using ports, objects can be compiled in such a way that the storage overhead of the associated stream can be eliminated, and that objects can have customised garbage collection behaviour.

We have shown that an EREW-PRAM can be simulated in AKL using ports, which suggests that most parallel algorithms can be expressed with reasonable efficiency.

We have demonstrated the usefulness of ports by showing how they can be used to emulate a variety of models of parallel programming, including Actors, Linda, and Dataflow networks. We have also shown how to implement shared passive objects and other synchronisation mechanisms.
