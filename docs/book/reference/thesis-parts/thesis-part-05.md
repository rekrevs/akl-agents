# Thesis Part 05

## PROGRAMMING PARADIGMS (continued)

dict(S) := leaf(S).

leaf([]) :-
  → true.
leaf([insert(K,V)|S]) :-
  → node(S, K, V, L, R),
    leaf(L),
    leaf(R).
leaf([lookup(K,V)|S]) :-
  → V = not_found,
    leaf(S).

node([], _, _, L, R) :-
  → L = [],
    R = [].
node([insert(K1, V1)|S], K, V, L, R) :-
  → ( K1 = K
    → node(S, K, V1, L, R)
    ; K1 < K
    → L = [insert(K1, V1)|L1],
      node(S, K, V, L1, R)
    ; K1 > K
    → R = [insert(K1, V1)|R1],
      node(S, K, V, L, R1) ).
node([lookup(K1, V1)|S], K, V, L, R) :-
  → ( K1 = K
    → V1 = found(V),
      node(S, K, V, L, R)
    ; K1 < K
    → L = [lookup(K1, V1)|L1],
      node(S, K, V, L1, R)
    ; K1 > K
    → R = [lookup(K1, V1)|R1],
      node(S, K, V, L, R1) ).

In the following section on object-oriented programming, we will relate this programming technique to conventional object-oriented programming and its standard terminology.

### 3.3 OBJECT-ORIENTED PROGRAMMING

In this section, the basic techniques that allow us to do object-oriented programming in AKL are reviewed. Like the programming techniques in the previous section, they are not a contribution of this dissertation, but belong by now to logic programming folklore.

There is more than one way to map the abstract concept of an object onto corresponding concepts in a concurrent constraint language. The first and most widespread of these will be described here in detail [Shapiro and Takeuchi 1983]. It is based on the process reading of logic programs. Several embedded languages have been proposed that support this style of programming (e.g., [Kahn et al. 1987; Yoshida and Chikayama 1988; Davison 1989]). They are typically much less verbose, and they also provide more explicit support for object-oriented concepts. More modern treatments of objects in concurrent logic languages exist (see, e.g., [Smolka, Henz, and Würtz 1993]). This is discussed in Chapter 7.

As will be seen, in this framework there is no real need for an *implementation* of objects, unlike the case when one is adding object-oriented support to a language such as C. Following an object-oriented style of programming is a very natural thing.

As a point of reference, we will adhere to the object-oriented terminology of [Snyder, Hill, and Olthoss 1989]. The meaning of this terminology will be summarised as it is introduced.

#### 3.3.1 Objects

An *object* is an abstract entity that provides *services* to its clients. Clients explicitly request services from objects. The request identifies the requested service, as well as the object that is to perform the service.

Objects are realised as processes that take as input a stream (a list) of requests. The stream identifies the object. The data associated with the objects are held in the arguments of the process. An object definition typically has one clause per type of request, which performs the corresponding service, and one clause for terminating (or deallocating) the object. Thus, clauses correspond to *methods*. The requests are typically expressions of the form name(A, B, C), where the constructor "name" identifies the request, and A, B, and C are the arguments of the request.

The process description, the agent definition, is the *class*, the implementation of the object. The individual calls to this agent are the *instances*.

A standard example of an object is the bank account, providing withdrawal, deposits, etc.

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

A computation starting with

make_bank_account(S),
S = [balance(B1), deposit(7), withdraw(3), balance(B2)]

yields

B1 = 0, B2 = 4

A bank account object is created by starting a process bank_account(S, 0) which is given as initial input an unspecified stream S (a variable) and a zero balance. The stream S is used to identify the object. A service deposit(5) is requested by binding S to [deposit(5)|S1]. The next request is added to S1, and so on.

In the above example, only one clause will match any given request. When it is applied, some computation is performed in its body and a new bank_account process replaces the original one. The requests in the above example are processed as follows. Let us start in the middle.

bank_account(S, 0), S = [deposit(7), withdraw(3), balance(B2)].

The bank account process is reduced by the clause matching the first deposit-request, leaving some computation to be performed.

N = 0+7, bank_account(S1, N), S1 = [withdraw(3), balance(B2)].

This leaves us with.

bank_account(S, 7), S1 = [withdraw(3), balance(B2)].

The rest of the requests are processed similarly.

Finally, there are a few things to note about these objects. First, they are automatically *encapsulated*. Clients are prevented from directly accessing the data associated with an object. In imperative languages, this is not as self-evident, as the object is often confused with the storage used to store its internal data, and the object identifier is a pointer to this storage, which may often be used for any purpose.

Second, requests are entirely *generic*. The expression that identifies a request may be interpreted differently, and may therefore involve the execution of different code, depending on the object. This does not involve mandatory declarations in some shared (abstract or virtual) ancestor class, as in many other languages.

Third, *becoming* another type of object is extremely simple. Instead of replacing itself with an object of the same type, an object may pass its stream, and appropriate parameters, on to a new object. An example of this was given in the section on process structures, where a leaf process became a node process when a message was inserted into a binary tree.

#### 3.3.2 Inheritance

In the object-oriented paradigm, objects can be classified in terms of the services they provide. One object may provide a subset of the services of another object. This way an *interface hierarchy* is formed.

It is of course important, from a software engineering point of view, that the descriptions of objects higher up in the hierarchy can be reused as parts of the descendant objects. This is called *implementation inheritance* or *delegation*.

Delegation is easily achieved in the framework we describe. However, since requests are completely generic, it is also possible to design an interface hierarchy without it, if so desired.

Delegation is achieved by creating instances of the ancestor objects. The object identifier of (the stream to) this ancestor object is held as an argument of the derived object. The object corresponding to the ancestor could appropriately be called a *subobject* of the derived object. The derived object filters incoming requests and delegates unknown requests to its subobject.

Delegation is not restricted to unknown requests. We may also define what is elsewhere known as *after-* and *before-methods* by filtering as well. The derived object may perform any action before passing a request on to a subobject.

Let us derive from the bank account class a kind of account that does some form of logging of incoming requests. Let us say that it also adds a get_log service that returns the log. This is easy.

make_logging_account(S) :=
  make_bank_account(O),
  make_empty_log(Log),
  logging_account(S, O, Log).

logging_account([get_log(L)|R], O, Log) :-
  → L = Log,
    logging_account(R, O, Log).
logging_account([Req|R], O, Log) :-
  → O = [Req|O1],
    add_to_log(Req, Log, Log1),
    logging_account(R, O1, Log1).
logging_account([], O, _) :-
  → O = [].

With delegation, it is cumbersome to handle the notion of *self* correctly. Modern forms of *multiple inheritance*, based on the principle of specialisation, are also difficult to achieve.

Instead, it is quite possible to view inheritance as providing the ability to share common portions of object definitions by placing them in superclasses, which are then implicitly copied into subclass definitions. To exploit this view, syntactic support has to be added to the language, e.g., along the lines of Goldberg, Silverman, and Shapiro [1992]. This view corresponds closely to that of conventional object-oriented languages.

#### 3.3.3 Ports for Objects

Ports are a special form of constraints, which, when added to AKL, or to any concurrent logic programming language, will solve a number of problems with the approach to object-oriented programming presented above. This section provides a preliminary introduction to ports. They, the problems they solve, and numerous examples of their use, are the topic of Chapter 7.

A *port* is a binary constraint on a bag (a multiset) of messages and a corresponding stream of these messages. It simply states that they contain the same messages, in any order. A bag connected to a stream by a port is usually identified with the port, and is referred to as a port. The open_port(P, S) operation relates a bag P to a stream S, and connects them through a port.

The stream S will usually be connected to an object. Instead of using the stream to access the object, we will send messages by adding them to the port. The send(M, P) operation sends a message M to a port P. To satisfy the port constraint, a message sent to a port will immediately be added to its associated stream, first come first served.

When a port is no longer referenced from other parts of the computation state, when it becomes garbage, it is assumed that it contains no more messages, and its associated stream is automatically closed. When the stream is closed, any object consuming it is thereby notified that there are no more clients requesting its services.

Thus, to summarise: A port is created with an associated stream (to an object). Messages are sent to the port, and appear on the stream in any order. When the port is no longer in use, the stream is closed, and the object may choose to terminate.

A simple example follows.

open_port(P, S), send(a, P), send(b, P)

yields

P = 〈a port〉, S = [a,b]

Here we create a port and a related stream, and send two messages. The messages appear in S in the order of the send operations in the composition, but it could just as well have been reversed. The stream is closed when the messages have been sent, since there are no more references to the port.

Ports solve a number of problems that are implicit in the use of streams. The following are the most obvious.

• If several clients are to access the same object, their streams of messages have to be merged into a single input stream. With ports, no merger has to be created. Any client can send a message on the same port.

• If objects are to be embedded in other data structures, creating e.g. an array of objects, streams have to be put in these structures. Such structures cannot be shared, since several messages cannot be sent on the same stream by different clients. However, several messages can be sent on the same port, which means that ports can be embedded.

• With naive binary merging of streams, message sending delay is variable. With ports, message sending delay is constant.

• Objects based on streams require that the streams are closed when the clients stop using them. This is similar to decrementing a reference counter, and has similar problems, besides being unnecessarily explicit and low-level. A port is automatically closed when there are no more potential senders, thus notifying the object consuming messages.

These and other problems and solutions are discussed in Chapter 7.
