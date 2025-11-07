# Thesis Part 16

## PORTS FOR OBJECTS (continued)

```
                 vector of ports
                       ↓
    ○───→  ╔═╗─┬─○
    ○───→  ║ ║─┼─○  "memory cell
"processor      ║ ║─┼─○   processes"
processes"      ║ ║─┼─○
    ○───→  ╚═╝─┴─○
           ↑
      parallel
   random access
```

**Figure 7.11. PRAM with AKL ports**

```
cellproc([], _) :-
  → true.
cellproc([read(V₀)|S], V) :-
  → V₀ = V,
    cellproc(S, V).
cellproc([write(V)|S], _) :-
  → cellproc(S, V).
cellproc([exch(V₁, V₂)|S], V) :-
  → V₂ = V,
    cellproc(S, V₁).
```

PRAM is achieved by creating an array of ports to cells (Figure 7.11).

```
memory(M) :=
  M = m(C₁, …, Cₙ),
  cell(C₁), …, cell(Cₙ).
```

Any number of processes can share this array and send messages to its memory cells in parallel, updating them and reading them. The random access is achieved through the random access to slots in the array, and the fact that we can send to embedded ports without updating the array. Sequencing is achieved by the processors, using continuations as above.

#### 7.6.2 RAM without Ports

Most logic programming languages do not even allow modelling RAM, as a consequence of the single-assignment property. Shapiro's simulator for a RAM [1986b] depends on a built-in *n*-ary stream distributor to access cell processes in constant time as above. In KL1 the MRB scheme allows a vector to be managed efficiently, as long as it is single-referenced [Chikayama and Kimura 1987].

```
memory(M) := new_vector(M, n).
```

A program may access (read) the vector using the

```
vector_element(Vector, Position, Element, NewVector)
```

operation (which preserves the single-reference property). Sequencing is achieved through continuing access on *NewVector*. Similarly, a program may modify (write) the array using the

```
set_vector_element(Vector, Position, OldElement, NewElement, NewVector)
```

operation (which also preserves the single-reference property). Sequencing can be achieved as above.

#### 7.6.3 PRAM without Ports?

If MRB (or *n*-ary stream distributors) and multiway merging are available, they can be used to model PRAM, but with a significant memory overhead.

Each processor-process is given its own vector (or *n*-ary stream distributor) of streams to the memory cells. All streams referring to a single memory cell are merged. Sequencing is achieved as above. Thus we need O(*n m*) units of storage to represent a PRAM with *n* memory cells and *m* processors.

The setup of memories is correspondingly more awkward.

```
memories(M₁, …, Mₘ) :=
  memvector(M₁, C₁₁, …, C₁ₙ),
  …,
  memvector(Mₘ, Cₘ₁, …, Cₘₙ),
  cell(C₁), …, cell(Cₙ),
  merge(C₁₁, …, C₁ₘ, C₁),
  …,
  merge(Cₙ₁, …, Cₙₘ, Cₙ).

cell(S) :=
  cellproc(S, 0).

memvector(M, C₁, …, Cₙ) :=
  new_vector(M₁, n),
  set_vector_element(M₁, 1, _, C₁, M₂),
  …,
  set_vector_element(Mₙ, n, _, Cₙ, M).
```

Memory is accessed and modified as in a combination of the two previous models (Figure 7.12). A stream to a memory cell is accessed using the KL1 vector operations. A message for reading or writing the cell is sent on the stream, and the new stream is placed in the vector. An isomorphic structure can also be achieved using *n*-ary stream distributors.

```
           one vector
        per "processor"
    ○───→ ╔═╗
          ║ ║─┬─○─M─○
    ○───→ ║ ║─┼─○─M─○
          ║ ║─┼─○─M─○
    ○───→ ╚═╝─┴─○
                ↑
           multi-way
            mergers
```

**Figure 7.12. PRAM without AKL ports**

### 7.7 EXAMPLES

The first two examples, which are due to Barth, Nikhil, and Arvind [1991], exhibit the need for parallel random access functionality in a parallel programming language. The two examples, histogramming a tree of samples and graph traversal, exemplify basic computation structures common to many different settings.

Barth, Nikhil, and Arvind contrast random access solutions with pure functional programs, showing clearly that the former are an improvement both in terms of the total number of computation steps and in terms of the length of the critical path (in "maximally" parallel executions). The compared programs can be expressed in AKL with and without ports, respectively. Only the parallel random access solution with ports is shown; its alternatives without ports can be expressed in many different ways.

The next two sections discuss Lisp and Id, which are functional languages extended with side effect operations, and suggest how these extensions can be simulated in AKL using ports.

The last three sections discuss concrete formalisms for concurrent programming, Actors, Linda, and Erlang, and show how their particular modes of communication can be realised in AKL using ports.

#### 7.7.1 Histogramming a Tree of Samples

Given a binary tree in which the leaves contain numbers in the range 1, …, *n*, count the occurrences of each number.

In our solution, the counts for the numbers are in a PRAM memory of the kind defined above, which we assume has indices in the given range. The program traverses the tree, and, for each number found, increments the value in the cell with this index. The short circuit guarantees that all nodes have been counted before returning the memory.

```
hist(T, M) :-
  memory(M₀),
  count(T, M₀, S₀, S),
  ( S₀ = S → M = M₀ ).

count(leaf(I), M, S₀, S) :-
  → arg(I, M, C),
    send((exch(K, K+1) & S = S₀), C).
count(node(L,R), M, S₀, S) :-
  → count(L, M, S₀, S₁),
    count(R, M, S₁, S).
```

#### 7.7.2 Graph Traversal

Given a directed graph in which the nodes contain unique identifiers and numbers, compute the sum of the numbers in nodes reachable from a given node. Assume that the identifiers are numbers in the range 1, …, *n*. Any number of computations may proceed concurrently on the same graph.

In our solution, the nodes which have been traversed are marked in a separate array. For simplicity we assume this to have the indices in the given range, whereas a better solution would employ hashing to reduce its size. Assume that the memory agent returns a memory of this size. A graph node is an expression of the form

```
node(I, K, Ns)
```

where *I* is the unique node identifier, *K* is a number (to be summed), and *Ns* is a list of neighbouring nodes. (Note that cyclic structures are not a problem in the constraint system of rational trees.)

```
sum(N, G, S) :=
  memory(M),
  traverse(N, G, M, S).

traverse(node(I, K, Ns), M, S) :-
  arg(I, M, C),
  send(exch(1, X), C),
  ( X = 1
  → S = 0
  ; traverse_list(Ns, M, S) ).

traverse_list([], _, S) :-
  → S = 0.
traverse_list([N|Ns], M, S) :-
  → traverse(N, M, S₁),
    traverse_list(N, M, S₂),
    S = S₁ + S₂.
```

#### 7.7.3 Lisp

Many functional languages, such as Standard ML, CommonLisp, and Scheme, support various forms of *side effect* operations. Whether or not such practices are regarded with favour by purists, they have allowed the integration of object-oriented capabilities, e.g., CLOS into CommonLisp, which substantially enhance Lisp's expressive power.

AKL models side effects using ports. If it is assumed that assignment is confined to variables and to explicitly declared objects, an efficient translation of such Lisp programs into AKL is possible, which apart from allowing reuse of old code may also be seen as a parallelisation technique for Lisp programs.

Assume given a translation of the functional subset of Lisp along the lines presented in the previous section. Assume further that an "object-oriented" extension provides operations of the form "(put-field x v)" and "(get-field x)" for the assignable fields of its objects. Clearly, these operations have to be serialised in the order given by sequential execution of Lisp, but other operations can remain unaffected.

The translation of a Lisp expression of the form

```
(r … (p …) (q …))
```

will yield an AKL program of the form

```
…, p(…, R₁), q(…, R₂), r(…, R₁, R₂, R₃), …
```

If we assume that p, q, and r perform side effect operations, these can be serialised by adding two new arguments, chained in the order of execution of the original Lisp program, e.g.,

```
…, p(…, R₁, T₀, T₁), q(…, R₂, T₁, T₂), r(…, R₁, R₂, T₂, T₃), …
```

These arguments are used to pass a token between side effecting operations; each of these will wait for the token, perform the operation, and then pass the token along. Assuming that assignable fields in objects are memory cells with continuation-calling ports as defined in Section 7.5.1, the put-field and get-field operations may be written as

```
put_field(record(…, X, …), V, token, T) :-
  → send((write(V) & T = token), X).

get_field(record(…, X, …), V, token, T) :-
  → send((read(V) & T = token), X).
```

It should be quite obvious how to proceed to a general translation of such features, and no space will be wasted on this exercise here.

As an aside, the concept of *monads* provides a clean explanation of some well-behaved forms of side effect programming (among many other things) [Wadler 1990]. It remains to be seen if it suffices to explain, e.g., CLOS.

#### 7.7.4 Id

In the taxonomy of Barth, Nikhil, and Arvind [1991], there are three approaches to parallel execution of functional languages:

*Approach A*: purely functional (strict or non-strict) with implicit parallelism or annotations for parallelism.

*Approach B*: strict functional, sequential evaluation, imperative extensions, and threads and semaphores for parallelism.

*Approach C*: non-strict functional, implicitly parallel evaluation, M-structures, and occasional sequentialisation.

The style of functional programming discussed in Section 3.4.1 provided parallelism in a manner adhering to Approach A. Erlang, discussed for its process-oriented features below, is reminiscent of Approach B. M-structures are available in the parallel non-strict functional language Id, which is the only instance of approach C [Barth, Nikhil, and Arvind 1991]. Note that the approach presented in the previous section does not fall within any of these categories.

M-structures give Id the ability to express arbitrary PRAM-algorithms, just as ports do for AKL. The port examples shown above were modelled upon corresponding Id programs, using techniques that do not immediately correspond to M-structures. In this section, we show that M-structures can easily be expressed in terms of ports.

An *M-field* is a value cell in a structured data type, such as a record or an array. Such a cell is either *full* or *empty*. The *take* operation atomically reads the contents of a full cell and sets its state to empty. If the cell is empty, the take operation suspends. The *put* operation atomically stores a value in an empty cell and sets its state to full. If there are suspended take operations, one is resumed. If the cell is full, the put operation suspends. A data structure that contains M-fields is called an *M-structure*.

An M-field can be modelled in AKL as follows.

```
make_mfield(P) :=
  open_port(P, S),
  separate(S, Ps, Ts),
  match(Ps, Ts).

separate([], Ps, Ts) :-
  → Ps = [], Ts = [].
separate([put(U)|S], Ps, Ts) :-
  → Ps = [U|Ps₁],
    separate(S, Ps₁, Ts).
separate([take(V)|S], Ps, Ts) :-
  → Ts = [V|Ts₁],
    separate(S, Ps, Ts₁).

match([], _) :-
  | true.
match(_, []) :-
  | true.
match([U|Ps], [V|Ts]) :-
  | V = U,
    match(Ps, Ts).
```

The state of the M-field (*full* or *empty*) is implicit in the program. If there are available put requests, then it is full, otherwise it is empty.

Since Id provides implicit parallelism, the order of take and put operations is quite undefined. The synchronisation provided by these operations is often sufficient, but sometimes general sequencing is desirable and Id provides a sequencing operator for this purpose. Here short-circuiting would be appropriate for a general translation of Id, whereas continuations (Section 7.5.1) are sufficient for most programming purposes.

Finally, note that the implementation scheme for ports described in Section 7.4.4 allows simple low-level optimisations, by which the efficiency of the implementation of a port-based object such as an M-structure can be radically improved. In this case, it would be possible to use the implementation techniques proposed for Id.

#### 7.7.5 Actors

The notion of Actors, as the intuitive notion of the essence of a process, was introduced by Hewitt and Baker [1977], and was further developed by Clinger [1981] and Agha [1986]. It was a source of inspiration for the conception of the concurrent logic programming languages. A mapping from Actors to AKL is shown here to make clear the ease with which the actor style of concurrency and "mailbox" communication can be expressed AKL.

A pure view of Actors is given. Any language based on Actors will be augmented by the power of the chosen host formalism, such as Scheme in the Act family of actor languages [Agha and Hewitt 1987], but the process-oriented component remains the same.

An *actor* has an *address* to which it may receive *communications*. We assume that communications have *identifiers* and *arguments*, which are the addresses of other actors. It has *acquaintances* which are other actors whose addresses it knows. Upon receiving a communication

1) communications may be sent in response, to its acquaintances or to arguments of the communication,

2) new actors may be created, with acquaintances chosen from the acquaintances of the actor, the arguments of the communication, and other new actors,

3) one of these actors may be appointed to receive further incoming communications on the original address.

An actor is defined by specifying its behaviour for each relevant type of communication.

It is easy to map the above notion of an actor into AKL using ports as follows. The definition of an actor *A* is mapped to a definition of an AKL agent *A*. The address of an actor is a port (*P*), which is created for each new actor, and to which communications are sent. The acquaintances (*F₁*, …, *Fₖ*) of *A* are supplied in the other arguments of *A*.

```
A(P, F₁, …, Fₖ) :=
  open_port(P, S),
  A_behaviour(S, F₁, …, Fₖ).
```

The agent A_behaviour consumes the corresponding stream (*S*) of communications, which are constructor expressions of the form m(*X₁*, …, *Xₗ*), where *Xᵢ* are ports. Its definition contains one clause of the following form for each possible type of incoming communication.

```
A_behaviour([m(X₁, …, Xₗ)|S], F₁, …, Fₖ) :-
  → send(M₁, …), …, send(Mₘ, …),
    new_N₁(P₁, …), …, new_Nₙ(Pₙ, …).
    A′_behaviour(S, …).
```

The communications sent are *M₁*, …, *Mₘ*. The new actors created are calls to new_*N₁*, …, new_*Nₙ*, and *A'* receives further communications on *S*. The communications sent, their arguments, and the remaining arguments to the atoms representing new actors, are chosen from *X₁*, …, *Xₗ*, *F₁*, …, *Fₖ*, *P₁*, …, *Pₙ*, as given by the original agent definition.

Each definition also contains a clause

```
A_behaviour([], F₁, …, Fₖ) :- | true.
```

which terminates the actor when its address, the port, is no longer accessible, and has been closed automatically.

A major deficiency of actor languages in comparison with AKL, and other concurrent (constraint) logic programming languages, is that the elegance of implicitly concurrent programs such as quicksort is not readily reconstructed in an actor-based language, with its explicit and strictly unordered form of communication.

#### 7.7.6 Linda

Linda is a general model for (asynchronous) communication over a conceptually shared data structure, the *tuple space* [Gelernter et al. 1985; Carriero and Gelernter 1989]. In a tuple space, *tuples* are stored, which are similar to constructor expressions in AKL, records in Pascal, and the like. There are three operations which access the tuple space. In the following *x* stands for an expression, *v* for a variable, and the notation {*X* | *Y*} for either *X* or *Y*.

```
out(x₁, …, xₙ)
```

stores a tuple in the tuple space,

```
in({var v₁ | x₁}, …, {var vₙ | xₙ})
```

retrieves (and deletes) a matching tuple from the tuple space, suspending if none was found, and

```
read({var v₁ | x₁}, …, {var vₙ | xₙ})
```

locates (but leaves in place) a matching tuple in the tuple space. The notation "var *v*" means that the variable is constrained by the matching operation; the other arguments should be equal to the corresponding slots in the matching tuple.

A tuple space can be modelled as an AKL agent. Processes access the tuple space using a port, the corresponding stream of which is consumed by the tuple space agent. The tuple space is represented as a list of tuples. Tuples may be arbitrary objects, which are selected using arbitrary conditions, not only plain matching. Note the use of the *n*-ary call primitive for this purpose. A condition is a constructor expression of the form

```
p(x₁, …, xₘ)
```

When "applied to" an argument using ternary call

```
call(p(x₁, …, xₘ), x, y)
```

it becomes the agent

```
p(x₁, …, xₘ, x, y)
```

which in the following context is expected to return a continuation on *y* if *x* satisfies the condition, otherwise it should fail. In general, all *n*-ary call operations may be regarded as implicitly defined by clauses

```
call(p(X₁, …, Xₘ), Y₁, …, Yₙ₋₁) :- → p(X₁, …, Xₘ, Y₁, …, Yₙ₋₁).
```

for all *p*/*n* type (program and constraint) atoms in a given program.

The tuple space server accepts a stream of "in" and "out" requests. ("read" is omitted, being a restricted form of "in".)

```
tuple_space([], _, []) :-
  → true.
tuple_space([in(P)|R], TS, S) :-
  → in(P, TS, S, TS₁, S₁),
    tuple_space(R, TS₁, S₁).
tuple_space([out(T)|R], TS, S) :-
  → out(T, R, TS, S, TS₁, S₁),
    tuple_space(R, TS₁, S₁).
```

The in request is served by scanning through the list of tuples looking for a matching tuple. If one is found, the continuation is called, otherwise the request is stored in the list of suspended requests.

```
in(P, [], S, TS₁, S₁) :-
  → TS₁ = [],
    S₁ = [P|S].
in(P, [T|TS], S, TS₁, S₁) :-
  call(P, T, C)
  → call(C),
    TS₁ = TS,
    S₁ = S.
in(P, [T|TS], S, TS₁, S₁) :-
  → TS₁ = [T|TS₂],
    in(P, TS, S, TS₂, S₁).
```

The out request is served by scanning through the list of suspended in requests looking for a matching request. If one is found, its continuation is called, otherwise the tuple is stored in the list of tuples.

```
out(T, TS, [], TS₁, S₁) :-
  → S₁ = [],
    TS₁ = [T|TS].
out(T, TS, [P|S], TS₁, S₁) :-
  call(P, T, C),
  → call(C),
    TS₁ = TS,
    S₁ = S.
out(P, TS, [M|S], TS₁, S₁) :-
  → S₁ = [M|S₂],
    out(P, TS, S, TS₁, S₂).
```

As a simple example, consider the following condition

```
integer(Y, X, C) :-
  integer(X)
  | C = (Y = X).
```

and the call

```
tuple_space([in(integer(Y)), out(a), out(1)], [], [])
```

which returns

```
Y = 1
```

after first suspending the in(integer(Y)) request, then trying it for out(a), which fails and is stored in the tuple space, then for out(1), which succeeds.

Of course, the above simple scheme can be improved. As it stands, the single tuple space server is a bottle neck, which unnecessarily serialises all accesses. This can be remedied, e.g., by distributing requests to different tuple-spaces using a hashing function, but the generality of the representation of conditions and tuples in the above program would have to be sacrificed to allow for a suitable hashing function.

```
send(Request, TupleSpace) :=
  hash(Request, Index),
  arg(Index, TupleSpace, Port),
  send(Request, Port).
```

To each port in the hash table is connected a tuple space server of the kind presented above.

#### 7.7.7 Erlang

Erlang is a concurrent functional programming language designed for prototyping and implementing reliable (distributed) real time systems [Armstrong, Williams, and Virding 1991; 1993]. Although not restricted to this area, Erlang is mainly intended for telephony applications. That functional programming as in the functional component of Erlang is provided by AKL is illustrated in Section 3.4.1. Here it is argued that the process component, including its error handling capabilities, can be expressed in AKL.

Erlang is probably less well known than the other languages in this chapter. Therefore, its essential features will be summarised before the comparison.

Processes are expressed in a sequential first-order functional programming language. The functional component is augmented with side effect operations such as creation of processes and message passing. The operation which creates a process returns a process identifier (*pid*).

```
Pid = spawn(Module, Function, Arguments)
```

A process may acquire its own pid using the self() operation. Pids may be given global names, a feature not further discussed here. Messages are sent to a pid with the *Pid* ! *Message* operation. Messages are stored in the mail-box of a process, from which they may be retrieved at a later time using the *receive* operation

```
receive
    Pattern [when Guard] -> … ;
    …
    [after Time -> …]
end
```

If a message matching a pattern (and satisfying the guard condition) is found among the messages in the mail-box, it is removed. The pattern is a constructor expression with local variables which are bound to the corresponding components in the message. The last clause may specify a time-out, which will not be further discussed here.

For the purpose of error-handling, processes may be *linked*. [It is assumed that these links are unidirectional, i.e., from one process to another process.] Links may be added, deleted, and, in particular, associated with new processes atomically at the moment of spawning (in which case a link is established from the new process to the old process).

```
link(Pid)                                       (Pid to self)
link(Pid₁, Pid₂)                                (Pid₂ to Pid₁)
unlink(Pid)
unlink(Pid₁, Pid₂)
Pid = spawn_link(Module, Function, Arguments)   (Pid to self)
```

If an error occurs in a process, exit-signals are automatically sent to the processes it is linked to. A process which receives an exit-signal will by default also exit. This can be overridden, whereby signals are trapped, and converted to ordinary messages, which can be received in the usual fashion. A process may simulate an exit, and may also force another process to exit.

```
process_flag(trap_exit, [true/false])     (trap exit-signals)
receive {'EXIT', From, Message} -> … ; end
exit(Reason)
exit(Pid, Reason)                          (force exit of Pid)
```

Signals may also be caught and generated by catch and throw operations, which are not further discussed here.

We now proceed to express the above in AKL using ports. To simplify the presentation, the following restrictions are made:

• Communication in Erlang is somewhat similar to tuple space communication in Linda, which was discussed in the previous section, and does also provide for time-outs. This aspect is ignored here. Communication is sent on a plain port, and served in a first-come first-served manner.

• Erlang distinguishes between signals and messages. This distinction is not made here, where both are regarded as messages, as when trapping of exit signals is turned on in Erlang.

• In Erlang, processes notify linked processes also about successful completion. This can be detected using short-circuiting, as in Section 7.7.1, and is not deemed important here.

An Erlang process identifier is mapped to an AKL port. Erlang processes have coarser granularity than AKL processes; to each Erlang process corresponds a *group* of AKL processes, performing the computations corresponding to the (sequential) functional component of Erlang. It is assumed that the definitions of these processes are written in the flat committed choice subset of AKL, where the guards may only contain simple tests.

The processes in a group share a pair p(*V*, *P*), the first component of which is either unconstrained or equal to the constant "exit", and the second component of which is a port. It is assumed that all atoms, except the simple tests in guards, are supplied with this pair in an extra argument. If an error occurs in a primitive operation, a message of the form exit(*Reason*) is sent on the port in the pair, otherwise no action is taken. If the first component is equal to "exit", an operation may terminate with no further action.

All choice statements in the definitions of processes in a group should have an extra case which examines the state of the first component of the pair, terminating if it is equal to "exit".

```
p(…, F) :- …
…
p(…, p(exit,_)) :- | true.
```

A process group is created as follows.

```
group(Args, L, P) :=
  descriptor(P, Args, D),
  error_handler(L, P, S, F),
  processes(S, Args, F, P).
```

The *descriptor* agent forms a suitable descriptor of the current process to be sent to linked processes in the event of a failure, and may be defined in any appropriate manner. The definitions of the following agents are generic, and can be shared between all process groups.

```
error_handler(L, P, S, F) :=
  open_port(P, S),
  open_port(FP, Cs),
  F = p(_,FP),
  distribute(S, FP, Ms),
  linker(Cs, D, L).
```

The *distribute* agent filters out the *special* messages to the group, concerning linking and exiting, which are sent to the linker.

```
distribute([], C, Ms) :-
  → Ms = [].
distribute([special(C)|S], FP, Ms) :-
  → send(C, FP, FP₁),
    distribute(S, FP₁, Ms).
distribute([M|S], FP, Ms) :-
  → Ms = [M|Ms₁],
    distribute(S, FP, Ms₁).
```

The *linker* agent holds the list of pids (ports) of processes which the group is linked to, and an initial list (*L*) may be provided at the time of creation of the group, as in the spawn_link operation. It informs linked processes in the event of forced exit or failure.

```
linker([link(P₁)|Cs], D, L) :-
  → linker(Cs, D, [P₁|L]).
linker([unlink(P₁)|Cs], D, L) :-
  → delete(P₁, L, L₁),
    linker(Cs, D, L₁).
linker([exit(R)|Cs], D, L) :-
  → F = p(exit,_),
    send_ports(L, exit(D, R)).

send_ports([], _) :-
  → true.
send_ports([P|Ps], M) :-
  → send(M, P),
    send_ports(Ps, M).
```

The *processes* agent creates the group of processes that perform the actual computation, receive and send messages, etc.

As demonstrated above, the degree of safety achieved with Erlang error handling can also be achieved in AKL, but at the cost of slightly more verbose programs: some process group set-up, and an extra argument to all atoms. Syntactic sugar could easily remedy this, if such capabilities warrant special treatment.

The computational overhead is the time to set up the error handler and the storage it occupies, the filtering of messages, the passing around of the extra argument, and (occasionally) suspending on the extra argument.

As it is currently defined, the error_handler will remain also when computation in the corresponding process group has terminated. It will terminate only when there are no more references to the port of the group.

### 7.8 DISCUSSION

The notion of ports provides AKL with a model of mutable data, which is necessary for effective object-oriented programming and effective parallel programming. Ports can serve as an efficient interface to foreign objects, e.g., objects imported from C++. They also support a variety of process communication schemes. Thus, ports reinforce otherwise weak aspects of AKL, and can also be introduced and used for the same purpose in other concurrent (constraint) logic programming languages.

Other useful communication mechanisms are conceivable that do not fall into the category discussed here. For example, the *constraint communication* scheme of Oz [Smolka, Henz, and Würtz 1993], has similar expressive power (except for automatic closing), but has to be supported by the additional concept of a blackboard. It can, however, be emulated by ports, using code equivalent to that for M-fields in Id (Section 7.7.4).
