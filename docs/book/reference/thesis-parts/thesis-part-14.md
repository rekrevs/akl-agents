# Thesis Part 14

## AN ABSTRACT MACHINE (continued)

Palmer and Naish [1991] describe NUA-Prolog, an and-parallel implementation of Prolog based on the BAM, otherwise similar to Andorra-I, but without a preprocessor guaranteeing consistency with sequential Prolog behaviour.

Bahgat [1991] describes an abstract machine for Pandora, an extension of PARLOG based on the BAM, an extension of the JAM with a choice-point stack for sharing. The model does not maintain the order of goals, which are not in linked lists. Instead, programmer intervention is required for the choice of a goal for a don't know nondeterministic step.

---

# CHAPTER 7

## PORTS FOR OBJECTS

Can object-oriented programming be realised effectively in AKL? This chapter is an introduction to the concept of *ports*. It is argued that ports are highly useful for object-oriented programming in a concurrent logic programming language. It is also illustrated by examples how to use ports to emulate other communication schemes for concurrent programming.

### 7.1 INTRODUCTION

We regard *objects* for concurrent logic programming languages as processes, as first proposed by Shapiro and Takeuchi [1983], and later extended and refined in systems such as Vulcan [Kahn et al 1987], A'UM [Yoshida and Chikayama 1988], and Polka [Davison 1989] (Figure 7.1).

Some of these systems are embedded languages that only make restricted use of the underlying language. We are interested in full-strength combinations of the underlying language with an expressive concurrent object-oriented extension, with all the problems and opportunities this entails.

In this chapter we introduce ports, an alternative to streams, as communication support for object-oriented programming in concurrent constraint logic programming languages. From a pragmatic point of view, ports provide efficient many-to-one communication, object identity, means for garbage collection of objects, and opportunities for optimised compilation techniques for concurrent objects. It will also provide us with means for mixing freely objects and other data structures provided in concurrent logic programming languages. From a semantic point of view, ports preserve the monotonicity of the constraint store, which is a crucial property of all concurrent constraint languages.

In the next few sections we present some of the background of our work. First we examine some requirements on object-oriented systems. Then we discuss the notion of a communication medium, and review a number of proposals that do not meet our requirements.

```
m m
m
m
m
m m
m
m m
m m
m
m
m
…

p([M|S], X) :-
  transform(M, X, X1),
  p(S, X1)
p([], X) :-
  terminate(X).
```

**Figure 7.1. Objects as stream-consuming recursive processes**

In the following sections, we introduce ports as a solution to these problems, one which also adds entirely new possibilities, such as a simple approach to optimised compilation of objects. We will also show that the Exclusive-read, Exclusive-write PRAM model of parallel computation can be realised quite faithfully in terms of space and time complexity using ports. This indirectly demonstrates that arbitrary parallel algorithms can be expressed quite efficiently.

Finally, a number of examples illustrate how the functionality of many other languages is captured by ports.

### 7.2 REQUIREMENTS

Our starting point is a number of requirements on object-oriented languages, and we will let these guide our work. In so doing, we here only consider requirements on the object-based functionality, including requirements on the interaction between the host language and the concurrent object-oriented extension. Other aspects of object-oriented languages, such as inheritance, can be handled in many different ways (e.g., [Goldberg and Shapiro 1992]).

Since our goal is the integration of concurrent object and logic programming, we conform to the tradition of languages such as C++, where the object-oriented aspects are added to the underlying language and, in particular, allow objects and other data structures to be mingled freely. Thus, in a logic programming language, it should be possible to have objects *embedded* in a term data structure (Figure 7.2, next page). Since terms can be shared freely, this will allow concurrent objects to share, for instance, an array of concurrent objects.

Higher-level object-oriented languages provide automatic *garbage collection* of objects that are no longer referenced (Figure 7.3, next page). Programmers do not have to think about when objects are no longer in use, nor do they have to deallocate them explicitly. The high-level nature of logic programming languages makes it desirable to provide garbage collection of objects, just as of other data structures.

```
        distribution of embedded
        references to objects
               ↓
              [·]───────○
               │
               └─[·]─┬─○
                  └──┼─○
                     └─○
```

**Figure 7.2. Objects embedded in terms**

```
         ○─────○──○
        ╱       ╲
       ○    ✗    ○
      ╱  disconnected  ╲
     ╱     object      ╲
    ○                   ○
```

**Figure 7.3. Garbage-collectable disconnected object**

Note that, in the goal-directed view of logic programming, an object is a goal, which has to be proved; it cannot just be thrown away. However likely the assumption that a goal is provable without binding variables, it cannot be taken for granted. In a concurrent object-oriented setting, another dimension is added, in that an object may still be active, and affect its environment, although it is no longer referred to by other objects. Even if there are no incoming messages, an object may wish to perform some cleaning up before being discarded. Thus, garbage collection of objects in concurrent (logic) programs should involve notifying an object that it will no longer receive messages. It is up to the object to decide to terminate.

In addition, an implementation of objects should provide

• *light-weight* message sending and method invocation, the cost of which should preferably be similar to that of a procedure call,

• *compact* representation of objects, the size of which should be dominated by the representation of instance variables,

• *conservation* of memory, which means that a state transition should only involve modifying relevant instance variables—objects are reused.

This chapter will address all but the last of the above requirements. The last requirement is generally solved in concurrent logic programming languages by providing a mechanism for detecting single references, and reusing the old instance variables.

### 7.3 COMMUNICATION MEDIA

A *communication medium* is a data type that carries messages between processes acting as objects (Figure 7.4).

```
                    communication
                       medium
  senders    ───→        ↓      ←─── receiver
of messages  ───→       ╱│╲     ←─── of messages
             ───→      ╱ │ ╲
                    references
                   to same medium
```

**Figure 7.4. The communication medium**

The medium is used as a handle to an object, and is regarded as the *object identifier* from a programmer's point of view.

In the concurrent constraint view that we take, the communication medium is managed (described and inspected) using constraints. All constraints are added to a shared constraint store. To send a message means to impose a constraint on the medium which allows a process to detect the presence of a message, and receive the message by inspecting its properties. A sender of messages is a *writer* of the medium. A receiver of messages is a *reader* of the medium. A message that is received once and for all is said to be *consumed*. A medium can be *closed* by imposing constraints that disallow additional messages. A receiver can detect that a medium is closed.

An important property of the constraint store in concurrent (constraint) logic programming languages is that it is monotone. Addition of constraints will produce a new constraint store that entails all the information in the previous one. This property is important because it implies that once a process is activated by the receipt of a message, this activation condition will continue to hold until the message is consumed, regardless of the actions of other processes.

The discussion above leads us to the following requirements on our communication medium.

• A solution should preserve the monotonicity of the constraint store.

• The number of operations required to send a message (make it visible to an end receiver) should be constant (for all practical purposes), independent of the number of senders. All senders should be given equal opportunity, according to a first come first served principle. We call this the *constant delay property*.

• When a part of the medium that holds a message has been consumed, it should be possible to deallocate or reuse the storage it occupied, by garbage collection or otherwise.

• To provide completely automatic garbage collection of objects, it should be possible to apply the closing operation automatically (when the medium is no longer in use).

• To enable sending multiple messages to embedded objects, it should be possible to send multiple messages to the same medium.

The last requirement seems odd in conventional object-oriented systems. It is however a problem in all single-assignment languages including the current (constraint) logic programming languages.

In the remainder of this section we discuss a number of communication media that have been proposed for concurrent (constraint) logic programming languages.

#### 7.3.1 Streams

The list is by far the most popular communication medium in concurrent logic programming. In this context lists are usually called *streams*. A background to streams and techniques for binary merging (see below) is given by Shapiro and Mierowsky [1984].

A message *m* is sent on the stream *S* by binding *S* to a list pair, *S* = [*m*|*S'*]. The next message is sent on the stream *S'*. A stream *S* is closed by making it equal to the empty list, *S* = []. A receiver, which should be waiting for *S* to become equal to either a list pair or the empty list, will then either successfully match *S* against a list pair [*M*|*R*], whereupon *M* will be made equal to *m* and *R* to *S'*, in which case the next message can be received on *S'*, or match *S* against [], in which case no further messages can be received. By the *end-of-stream* we mean a tail of the stream that is not yet known to be a list pair or an empty list.

To achieve the effect of several senders on the same stream, there are two basic techniques: (1) The stream is split into several streams, one for each sender, which are interleaved into one by a merger process. (2) The language provides some form of atomic "test-and-set" unification, which allows multiple writers to compete for the end-of-stream.

*Merging* is typically achieved either by a tree of binary mergers, or by a multiway merger. The binary-merge tree is built by splitting a stream as necessary (Figure 7.5).

```
         binary
         mergers
           ↓
    ○──M──○
    ○─╱ ╲─○
      M   M
         ╱ ╲
        ○   ○
```

**Figure 7.5. Binary merge network**

Clearly, this technique does not have the constant delay property as the (best case) cost is O(log *m*) in the number of senders *m*. A *multiway merger* is a single merger process that allows input streams to be added and deleted dynamically (Figure 7.6).

```
           multi-way
             merger
    ○───→     ↓
    ○───→    ╱M╲──→○
    ○───→   ╱  │  ╲
    ○───→  ╱   │   ╲
```

**Figure 7.6. Multiway merger**

A *constant delay multiway merger* cannot be expressed in most concurrent logic programming languages (AKL is an exception), but it is conceptually clean, and it is quite possible to provide one as a language primitive. We will assume that all multiway mergers have constant delay.

A number of disadvantages of merging follow.

• A merger process has to be created whenever there is the slightest possibility that several senders will send on the same stream. For many purposes this is not a problem. Once a multiway merger is created, adding and deleting input streams is fairly efficient. Yet, needing one feels like an overhead in an object-oriented context, where references to objects should be freely distributable.

• Explicit closing of all streams to all objects is necessary, since otherwise, either the program will eventually deadlock, or some objects will continue to be suspended, forever occupying storage.

• The merger process itself occupies storage, it also wastes storage when creating a new merged stream, and if it uses the standard mechanisms for suspension, it is likely to be (comparatively) inefficient.

• Messages have to be sent on the current end-of-stream variable, which is changed for every message sent. Assume that a process sends messages on streams which are embedded in a data structure, e.g., a tree containing objects. When a message has been sent on one stream, the new end-of-stream variable has to be "put back" into the tree. Usually this means building a new version of the tree, with the new end-of stream in place of the old, possibly reusing some unaffected parts of the old tree but necessarily allocating some new nodes. (However, if some form of single reference optimisation is employed by the language implementation new parts may reuse old storage: e.g., [Chikayama and Kimura 1987; Kahn and Saraswat 1990; Foster and Winsborough 1991]). Even worse, if this tree is to be shared with another process, where the possibility of the other process sending messages on the embedded streams cannot be excluded, two copies of the tree have to be created (allocating new nodes for at least one). All the streams have to be split in two (by merging), one for each copy.

• A serious problem is the *transparent message-distribution problem*. A message is usually a term *m*(*C₁*, …, *Cₙ*) where the *Cᵢ*'s are message components. Suppose we want to implement a transparent message-distributor object, which when it receives a message, of any kind, will distribute it to a list of other objects. Without prior knowledge of the components of messages, the distributor object cannot introduce the merging required for stream components.

An advantage with merging is that it allows list pairs to be reused in the merger and deallocated by the receiver as soon as a message has been consumed. In some cases, in a system with fairly static object-structure, explicit closing of all streams as a means for controlled termination of objects can be considered an advantage. Another general advantage of all stream communicating systems is the implicit sequencing of messages from a source object to a destination. This simplifies synchronisation in many applications.

*Multiple writers* can only be expressed in some languages with atomic "test-and-set" unification. The drawbacks of multiple writers are summarised as follows:

• The cost for multiple writers is typically O(*m*) per message, when there are *m* senders, and is therefore even further from the constant delay property than merging.

• The delay is proportional to the number of messages that have been sent.

• An inactive sender may hold a reference to parts of the stream that have already been consumed by the intended receiver, making deallocation impossible.

• It is difficult to close a stream. Some termination detection technique, such as *short-circuiting* (see, e.g., [Shapiro 1986a]), has to be used. In practice, this outweighs the advantages of multiple writers.

An advantage of multiple writers is that no merger has to be created. Moreover, several messages can be sent on the same stream, and not only by having explicit access to the end-of-stream variable.

*Neither* merging *nor* multiple writers provides a general solution to the problem of automatic garbage collection of objects. There are only special cases, as exemplified by A'UM [Yoshida and Chikayama 1988].

#### 7.3.2 Multiway Merging

A constant delay merger written in AKL is presented in this section, as an indication of its expressiveness, and to illustrate a novel programming technique: *bagof with feedback*.

The way multiway mergers are normally used, a process requests a new input stream from the merger on one of the existing input streams, either by sending a special message which is caught by the merger, e.g., *S* = [new_stream(*S₁*)|*S₂*], or by generalising streams to *stream trees* using an additional constructor, e.g., *S* = split(*S₁*, *S₂*), which is the solution adopted here.

```
          a
         ╱ ╲
        b   e
       ╱ ╲ ╱ ╲
      c  d f  g     ⟺    [a,b,e,g|_]
```

**Figure 7.7. A stream tree with corresponding merged stream**

In Figure 7.7, the relation between a (stylised) stream tree and a corresponding merged stream is shown. It is assumed that the merger has considered the messages up to the border drawn. Other messages (*c*, *d*, *f*) have been sent, i.e., are visible in the constraint store. It is easy to write a merger satisfying the first requirement using binary mergers, but the delay is then unbounded. The second requirement means that there should be a constant *k*, such that the number of computation steps required to detect the presence of and send the three known messages *c*, *d*, and *f*, should not exceed 3*k*.

To prepare the way for the complete solution, a simpler *ether* agent is shown first, which does not preserve the order of messages on input streams.

It is called with an input stream tree and an output stream. Ether uses unordered bagof to collect messages on the input streams (Figure 7.8).

```
ether(T, S) :=
  unordered_bagof(M, tree_member(M, T), S).
```

```
          a
         ╱ ╲
        b   c     →  [a,b,c|_]
                  ↑
         tree_member
              ↑
        unordered_bagof
```

**Figure 7.8. Ether merging**

```
tree_member(M, split(S1, S2)) :-
  → ( true ? tree_member(M, S1)
    ; true ? tree_member(M, S2) ).
tree_member(M, [M1|S]) :-
  → ( M = M1 ? true
    ; true ? tree_member(M, S) ).
tree_member([], Y) :-
  → fail.
```

The declarative reading of tree_member is that the message in its first argument is a member of the tree formed by split nodes and stream nodes in its second argument. The unordered bagof agent will collect these members to the output stream, when they become available along the fringe of this tree.

However, since the elements of an input stream will appear as different solutions, the unordered bagof may collect them in an arbitrary order, and thus their order is not preserved on the output stream. To remedy this, two processes are necessary, one bagof-based process to collect input streams, and one that guarantees sequentialisation of messages.

The improved merger consists of two co-operating agents: (1) a bagof agent that collects streams on which messages have been sent, and (2) a server that for each stream either removes a message (and feeds the rest to the generator in the bagof agent), splits the stream (and feeds the two new streams to the generator), or closes the stream (Figure 7.9).

```
merger(S0, S) :=
  server(B, A, S, 1),
  unordered_bagof(I, generator(I, [S0|A]), B).
```

The generator process enters the streams as alternative solutions for the bagof agent. It constrains the streams to be known to have one of the possible forms, [], [_|_], or split(_,_), before they are passed to the server.

```
              server
                ↓
           unordered_bagof
              ╱ │ ╲
             ○  ○  ○  ●
            ╱   ↑   ↑
    stream "guards"  generator
```

**Figure 7.9. Constant delay multiway merger in AKL**

```
generator(I, [S|R]) :-
  → ( true
    ? I = S,
      ( I = [] → true
      ; I = [_,_] → true
      ; I = split(_,_) → true )
    ; true
    ? generator(I, R) ).
```

The server agent expects a stream of instantiated streams from the bagof agent. Apart from dealing with these in the above mentioned way, it also keeps track of the number of merged streams, and closes the output stream and terminates the bagof agent when none remain.

```
server([[E|R]|B], A, S, N) :-
  | S = [E|S1],
    A = [R|A1],
    server(B, A1, S1, N).
server([split(S1,S2)|B], A, S, N) :-
  | A = [S1,S2|A1],
    server(B, A1, S, N+1).
server([[]|B], A, S, N) :-
  | server(B, A, S, N-1).
server(B, A, S, 0) :-
  | A = [],
    S = [].
```

The first clause deals with the "normal" case, when a message has been sent on the input stream. The message is added to the output stream, and the rest of the input stream is fed back to the generator. The second clause deals with the case when an input stream is split in two. Both are fed back to the generator, and the number of input streams is incremented by one. The third clause deals with the case when an input stream is closed. The number of input streams is decremented by one. The fourth clause deals with the case when there are no remaining input streams. The output stream is closed, and [] is fed back into the generator to thereby terminate the bagof.

This section presented an implementation of a multiway merger in AKL, but does not provide new solutions to the problems discussed in the previous section. It can be suspected that the problems of merging and multiple-writers are inherent in streams, and people have therefore looked for alternative data types.

#### 7.3.3 Mutual References

Shapiro and Safra [1986] introduced *mutual references* to optimise multiple writers, and as an implementation technique for constant delay multiway merging. The mental model is that of multiple writers.

A shared stream *S* is accessed indirectly through a mutual reference *Ref*, which is created by the allocate_mutual_reference(*Ref*, *S*) operation. Conceptually, the mutual reference *Ref* becomes an alias for the stream *S*. The message sending and stream closing operations on mutual references are provided as built-in operations. The stream_append(*X*, *Ref*, *New_Ref*) operation will bind the end-of-stream of *Ref* to the list pair [*X*|*S'*], returning *New_Ref* as a reference to the new end-of-stream. The stream *S* can be closed using the close_stream(*Ref*) operation, which binds the end-of-stream to the empty list [].

An advantage of this is that the mutual reference can be implemented as a pointer to the end-of-stream. When a message is appended, the pointer is advanced and returned. If a group of processes are sending messages on the same stream using mutual references, they can share the pointer, and sending a message will always be an inexpensive, constant time operation. Mutual references can be used to implement a constant delay multiway merger. Another advantage is that an inactive sender will no longer have a reference to old parts of the stream. This makes it possible to deallocate or reuse consumed parts of the stream.

A disadvantage is that we cannot exclude the possibility that the stream has been constrained from elsewhere, and that stream_append has to be prepared to advance to the real end-of-stream to provide multiple writers behaviour.

Otherwise, mutual references has the advantages of multiple writers, but the difficulty of closing the stream remains. It is also unfortunate that the mental model is still that of competition instead of co-operation.

#### 7.3.4 Channels

Tribble et al [1987] introduced *channels* to allow multiple readers as well as multiple writers.

A channel is a partially ordered set of messages. The write(*M*, *C₁*, *C₂*) operation imposes a constraint that: (1) the message *M* is a member of the channel *C₁*, and (2) *M* precedes all messages in the channel *C₂*. The read(*M*, *C₁*, *C₂*) operation selects a minimal (first) element *M* of *C₁*, returning the remainder as the channel *C₂*. The empty(*C*) operation tests the channel *C* for emptiness. The close(*C*) operation imposes a constraint that the channel *C* is empty.

In the intended semantics, messages have to be labelled to preserve message multiplicity, and only minimal channels (without superfluous messages) satisfying the constraints are considered.

Channels seem to share most of the properties of multiple writers on streams. Thus, all messages have to be retained on an embedded channel, in case it will be read in the future. An inactive sender causes the same problem. Closing is just as explicit and problematic. The multiple readers ability can be achieved by other means. For example, a process can arbitrate requests for messages from a stream conceptually shared by several readers.

#### 7.3.5 Bags

Kahn and Saraswat [1990] introduced *bags* for the languages Lucy and Janus.

Bags are multisets of messages. There is no need for user-defined merging, as this is taken care of by the Tell constraint bag-union *B* = *B₁* ∪ … ∪ *Bₙ*. A message is sent using the Tell constraint *B* = {*m*}. A combination of these two operations, *B* = {*m*} ∪ *B'*, corresponds to sending a message on a stream, but without the order of messages being given by the stream. A message is received by the Ask constraint *B* = {*m*} ∪ *B'*.

Note that bags can be implemented as streams, with a multiway merger as bag-union. Therefore, it is not surprising that bags have most of the disadvantages of streams with multiway merging. The host languages Lucy and Janus only allow single-referenced objects and therefore suffer less from these problems. It is possible to reuse list pairs in the multiway merger, and to deallocate (or reuse) list pairs when a bag is consumed.

### 7.4 PORTS

We propose *ports* as a solution to the problems with previously proposed communication media. In this section, we first define their behaviour. We then discuss their interpretation. Finally, we describe their implementation.

#### 7.4.1 Ports Informally

A *port* is a connection between a bag of messages and a corresponding stream of these messages (Figure 7.10, next page). A bag which is connected via a port to a stream is usually identified with the port, and is referred to as a port.

The open_port(*P*, *S*) operation creates a bag *P* and a stream *S*, and connects them through a port. Thus, *P* becomes a port to *S*. The send(*M*, *P*) operation adds a message *M* to a port *P*. A message which is sent on a port is added to its associated stream with constant delay. When the port becomes garbage, its associated stream is closed. The is_port(*P*) operation recognises ports.

```
        AKL port
           ↓
    ○──→  ╔═╗
    ○──→  ║P║──→○
    ○──→  ╚═╝
    ○──→
```

**Figure 7.10. An AKL port**

A first simple example follows. Given the program

```
p(S) := open_port(P, S), send(a, P), send(b, P).
```

calling

```
p(S)
```

yields the result

```
S = [a,b]
```

Here we create a port and a related stream, and send two messages. The order in which the messages appear could just as well have been reversed.

Ports solve all of the problems mentioned for streams and others.

• No merger has to be created; a port is never split.

• Several messages can be sent on the same port, which means that ports can be embedded.

• Message sending delay is constant.

• Senders cannot refer to old messages and thus prevent garbage collection.

• A port is closed automatically when there are no more potential senders.

• The transparent forwarding problem is solved, since messages can be distributed without inspection.

• Messages can be sequenced, as described in Section 7.5.

#### 7.4.2 Ports as Constraints

We can provide a sound and intuitive interpretation of ports as follows. Ports are bags (also called multisets). The open_port and send operations on ports are constraints with the following reading. The open_port constraint states that all members in the bag are members in the stream and vice versa, with an equal number of occurrences. The send constraint states that an object is a member of the bag. The is_port constraint states that ∃*S* open_port(*P*, *S*) for a bag *P*.

Our method for finding a solution to these constraints is don't care nondeterministic. Any solution will do. Therefore, the interpretation in terms of constraints is not a complete characterisation of the behaviour of ports, just as Horn clauses do not completely characterise the behaviour of commit guarded clauses. In particular, it does not account for message multiplicity, nor for their "relevance", i.e. it does not "minimise" the ports to the messages that appear in a computation.

A logic with resources could possibly help, e.g., Linear Logic [Girard 1987]. It can easily capture the don't care nondeterministic and resource sensitive behaviour of ports. The automatic closing requires much more machinery. If such an exercise would aid our understanding remains to be seen.

The method is also incomplete. A goal of the form

```
open_port(P1, S1), open_port(P2, S2), P1 = P2
```

or of the form

```
open_port(P1, S1), (open_port(P2, S2), P1 = P2 → …)
```

cannot be solved. With the constraint interpretation, this would amount to unifying *S₁* and *S₂*, but to be able to do this it is necessary to keep the whole list of messages sent throughout the lifetime of a port. Instead, this situation may be regarded as a runtime error.

Goals of the form

```
open_port(P, S), (send(M, P) → …)
```

or of the form

```
send(M, P), (open_port(P, S) → …)
```

can also not be solved. These situations do not make sense pragmatically, and are left unsolved. Both cases can be detected in an implementation, and could be reported as a runtime errors.

The AKL computation model does not, however, in its present form support don't care nondeterministic or incomplete constraint solving. An alternative approach is to describe the behaviour of port operations by *port reduction rules*, which assign unique identifiers to ports, and only make use of constraints for which it is reasonable for constraint solving to be complete [Franzén 1994]. This corresponds to the behaviour of the AGENTS implementation of ports, and is also the approach taken in the following formalisation.

#### 7.4.3 Port Reduction Rules

Following Franzén [1994], we define the operational semantics of ports in terms of rewrites on port operations and on (parts of) the local constraint store of an and-box, to which correspond D-mode transitions in the AKL computation model. The syntactic categories are extended so that port operations may occur in the position of a program atom in programs and in configurations.

In the following, the letter *n* stands for a natural number.

A constraint

```
port(v, n, w, w′)
```

means that *v* is a port with identifier *n*, associated list *w*, and *w'* is a tail of *w*. (To conform exactly with the syntax for constraints in the computation model, the parameter *n* would have to be expressed as a variable constrained to be equal to a number, but this is relaxed here.)

Observe that the following rules strictly accumulate information in the constraint store; the right hand side always implies the left hand side.

The first rule opens a port.

```
and(R, open_port(v, w), S)σᵥ ⇒ᴰ_χ and(R, S)ᵖᵒʳᵗ⁽ᵛ'ⁿ'ʷ'ʷ⁾∧σᵥ
```

where *n* is a closed term that does not occur in *R*, *S*, or χ.

The second rule enters a constraint that recognises ports.

```
and(R, is_port(v), S)σᵥ ⇒ᴰ_χ and(R, S)∃u∃w∃w′ port(v, u, w, w′)∧σᵥ
```

The third rule consumes a send to a port, moving the message to its associated stream. Observe that this rule monotonically adds information to the constraint store. Although the send constraint is removed, it is still implied by the presence of the message in the stream. Observe that the constraint store is composed by the associative and commutative conjunction operator (∧), and that constraint atoms may be reordered as appropriate for the application of rules.

```
and(R, send(u, v′), S)port(v, n, w, w′)∧σᵥ ⇒ᴰ_χ and(R, S)port(v, n, w, w″)∧w′=[u|w″]∧σᵥ
```

if σ∧env(χ) entails *v* = *v'*.

The fourth and final rule closes the associated stream when the port only occurs in a single port constraint.

```
and(R)port(v, n, w, w′)∧σᵥ ⇒ᴰ_χ and(R, S)w′=[]∧σᵥ
```

For this to become (nontrivially) applicable, it is necessary to have "garbage collection" rules. A simplification rule is given for a constraint theory **TC** with port constraints and rational tree equality constraints of the form *X* = *Y* or *X* = *f*(*Y₁*, …, *Yₙ*), where variables may be ports. (For a discussion about such combinations of theories, see [Franzén 1994].)

A *garbage collection rule* for this theory is

```
and(R)σᵤ ⇒ᴰ_χ and(R)θᵥ
```

where the constraint atoms in θ is a strict subset of those in σ such that

```
TC ⊨ env(χ) ⊃ (∃Wσ ≡ ∃Wθ)
```

where *W* contains all variables in *U* not occurring in *R*, and where the set *V* contains all variables in *U* that occur in θ or *R*. This rule is clearly terminating, since there is a finite number of constraint atoms in a local constraint store.

For example, if χ is λ, *R* is p(*X*), σ is *X* = f(*Y*) ∧ *Z* = g(*W*,*X*), and *U* is {*W*, *X*, *Y*, *Z*}, then θ is *X* = f(*Y*) and *V* is {*X*,*Y*}, since

```
TC ⊨ ∃W∃Y∃Z (*X* = f(*Y*), *Z* = g(*W*, *X*)) ≡ ∃W∃Y∃Z (*X* = f(*Y*))
```

This should correspond to our intuition for garbage collection.

#### 7.4.4 Implementation of Ports

The implementation of ports, and of objects based on ports, gives us other advantages, which are first discussed in this section and then returned to in Section 7.5.2.

The implementation of ports can rely on the fact that a port is only read by the open_port/2 operation, and that the writers only use the send/2 and is_port/1 operations on ports, which are both independent of previous messages.

Therefore, there is no need to store the messages in the port itself. It is only necessary that the implementation can recognise a port, and add a message sent on a port to its associated stream. This can be achieved simply by letting the representation of a port point to the stream being constructed. In accordance with the port reduction rules, adding a message to a port then involves getting the stream, unifying the stream with a list pair of the message and a new "end-of-stream", and updating the pointer to refer to the new end-of-stream. Closing the port means unifying its stream with the empty list. Note that the destructive update is possible only because the port is "write only".

In this respect, ports are similar to mutual references. But, for ports there is conceptually no such notion as advancing the pointer to the end-of-stream. We are constructing a list of elements in the bag, and if the list is already given, it is unified with what we construct.

Other implementations of message sending are conceivable, e.g. for distributed memory multiprocessor architectures.

That a port has become garbage is detected by garbage collection, as suggested by the definition. If a copying garbage collector is used, it is only necessary to make an extra pass over the ports in the old area after garbage collection, checking which have become garbage (i.e., were not copied). Their corresponding streams are then closed.

From the object-oriented point of view, this is not optimal, as an object cannot be deallocated in the first garbage collection after the port becomes garbage, which means that it survives the first generation in a two-generation generational garbage collector. Note that for some types of objects, this is still acceptable, as their termination might involve performing some tasks, e.g. closing files. For other objects, it is not. In the next section we discuss compilation techniques based on ports that allow us to differentiate between these two classes of objects, and treat them appropriately.

Reference counting is more incremental, and is therefore seemingly nicer for our purposes, but the technique is inefficient, it does not rhyme well with parallelism, and it does not reclaim cyclic structures. MRB [Chikayama and Kimura 1987] and compile-time GC (e.g., [Foster and Winsborough 1991]) are also of limited value, as we often want ports to be multiply referenced.

### 7.5 CONCURRENT OBJECTS

Returning to our main objective, object-oriented programming, we develop some programming techniques for ports, and discuss implementation techniques for objects based on ports.

Given ports, it is natural to retain the by now familiar way of expressing an object as a consumer of a message stream, and use a port connected to this stream as the object identifier.

```
create_object(P, Initial) :=
  open_port(P, S),
  object_handler(S, Initial).
```

In the next two sections we address the issues of synchronisation idioms, and of compilation of objects based on ports, as above.

#### 7.5.1 Synchronisation Idioms

We need some synchronisation idioms. How do we guarantee that messages arrive in a given order? We can use continuations, as in Actor languages.

The basic sequencing idiom is best expressed by a program. In the following, the call/1 agent is regarded as implicitly defined by clauses

```
call(p(X1, …, Xn)) :- → p(X1, …, Xn).
```

for all *p*/*n* type (program and constraint) atoms in a given program.

```
open_cc_port(P, S) :=
  open_port(P, S0),
  call_cont(S0, S).

call_cont([], S) :-
  → S = [].
call_cont([(M & C)|S0], S) :-
  → S = [M|S1],
    call(C),
    call_cont(S0, S1).
call_cont([M|S0], S) :-
  → S = [M|S1],
    call_cont(S0, S1).
```

The (*Message* & *Continuation*) operator guarantees that messages sent because of something that happens in the continuation will come after the message. For example, it can be used as follows.

```
open_cc_port(P, S), send((a & Flag = ok), P), p(Flag, P).
```

The agent *p* may then choose to wait for the token before attempting to send new messages on the port *P*.

The above synchronisation technique using continuations can be implemented entirely on the sender side, with very little overhead. A goal send((*m* & *C*), *P*) is compiled as (send(*m*, *P*), *C*), with the extra condition that *C* should only begin execution after the message has been added to the stream associated with the port. It should be obvious that this is trivial, even in a parallel implementation.

Another useful idiom is the three-argument send, defined as follows.

```
send(M, P0, P) :=
  send((M & P = P0), P0).
```

which is useful if several messages are to be sent in sequence. If this is very common, the send_list operation can be useful.

```
send_list([], P0, P) :-
  → P = P0.
send_list([M|R], P0, P) :-
  → send(M, P0, P1),
    send_list(R, P1, P).
```

Both, of course, assume the use of the above continuation calling definition.

#### 7.5.2 Objects based on Ports

If the object message-handler consumes one message at a time, feeding the rest of the message stream to a recursive call, e.g., as guaranteed by an object-oriented linguistic extension, then it is possible to compile the message handler using *message oriented scheduling* [Ueda and Morita 1992]. Instead of letting messages take the indirect route through the stream, this path can be shortcut by letting the message handling process pose as a special kind of port, which can consume its messages directly. There is then no need to save messages to preserve stream semantics. It is also easy to avoid creating the "top-level structure" of the message, with suitable parameter passing conventions. The optimisation is completely local to the compilation of the object.

Looking also at the implementation of ports from an object-oriented point of view, an object compiled this way poses as a port with a customised send-method. This view can be taken further by also providing customised garbage collection methods that are invoked when a port is found to have become garbage. If the object needs cleaning up, it will survive the garbage collection to perform this duty, otherwise the GC method can discard the object immediately.

Objects in common use, such as arrays, can be implemented as built-in types of ports, with a corresponding built-in treatment of messages. This may allow an efficient implementation of mutable data-structures. Ports can also serve as an interface to objects written in foreign languages.

Ports and built-in objects based on ports are available in AGENTS [Janson et al 1994]. An interesting example is that it provides an AKL engine is as a built-in object. A user program can start a computation, inspect its results, ask for more solutions, and, in particular, reflect on the failure or suspension of this computation. This facility is especially useful in programs with a reactive part and a (don't know nondeterministic) transformational part, where the interaction with the environment in the reactive part should not be affected by nondeterminism or failure, as exemplified by the AGENTS top-level and some programs with graphical interaction. In the future, this facility will also be used for debugging of programs and for metalevel control of problem solving.

In AGENTS, the open_port/2 operation is a specialised open_cc_port/2, which does not accept general continuations, but supports the send/3 operation.

### 7.6 PRAM

Shapiro [1986b] discusses the adequacy of concurrent logic programming for two families of computer architectures by simulating a RAM (Random Access Machine) and a network of RAMs in FCP (Flat Concurrent Prolog). However, a simulator for shared memory multiprocessor architectures, PRAMs (Parallel RAMs), is not given.

We conjecture that PRAMs cannot be simulated in concurrent logic programming languages without ports or a similar construct. This limitation could, among other things, mean that array-bound parallel algorithms, such as many numerical algorithms, cannot always be realised with their expected efficiency in these languages.

In the following we will show the essence of a simulator for an Exclusive-read Exclusive-write PRAM in AKL using ports.

#### 7.6.1 PRAM with Ports

A memory cell is easily modelled as an object.

```
cell(P) :=
  open_cc_port(P, S),
  cellproc(S, 0).
```
