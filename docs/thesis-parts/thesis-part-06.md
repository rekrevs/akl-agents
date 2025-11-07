# Thesis Part 06

## PROGRAMMING PARADIGMS (continued)

### 3.4 FUNCTIONS AND RELATIONS

Functions and relations are simple but powerful mathematical concepts. Many programming languages have been designed so that one of the available interpretations of a procedure definition should be a function or a relation. AKL has well-defined subsets that enjoy such interpretations, and provide the corresponding programming paradigms.

#### 3.4.1 Functions

The functional style of programming is characterised by the determinate flow of control and by the non-cyclic flow of data. There is no don't care or don't know nondeterminism—a single result is computed. Agents do not communicate bidirectionally—an agent takes input from one agent and produces output to another agent. The latter point is weakened somewhat if the language has a non-strict semantics, in which case "tail-biting" techniques are possible.

Many of the AKL definitions are indeed written in the functional style. For example, the "append", "squares" and "fruits" definitions in the preceding sections are essentially functional, although the latter two were introduced as components in a process-oriented setting.

The basic relation between functional programs and AKL definitions may be illustrated by an example, written in the non-strict, purely functional language Haskell [Hudak and Wadler 1991]. (The appropriate type declarations are supplied with the functional program for clarity.)

data (BinTree a) => (Leaf a) | (Node (BinTree a) (BinTree a))
flatten :: (BinTree a) -> [a] -> [a]
flatten (Leaf x) l = x:l
flatten (Node x y) l = flatten x (flatten y l)

In AKL, a corresponding program may be phrased as follows.

flatten(leaf(X), L, R) :-
  → R = [X|L].
flatten(node(X, Y), L, R) :-
  → flatten(Y, L, L1),
    flatten(X, L1, R).

The main differences are that an explicit argument has to be supplied for the output of the "function", and that nested function applications are unnested, making the output of one call the input of another.

AKL is not a higher-order language, but can provide similar functionality in a simple manner. The technique has been known in logic programming for a long time [Warren 1982; Cheng, van Emden, and Richards 1990]. A term representation is chosen for each definition in a program, and an agent apply is defined, which given such a term applies it to arguments and executes the corresponding definition.

Let a term p(n, t1, …, tm) represent a definition p/(n-m), which when applied to n-m arguments tm+1, …, tn calls p/n with p(t1, …, tn).

To give an example relating to the above programs, the term flatten(3) corresponds to the function flatten, and the term flatten(3, Tree) to the function (flatten tree) (where Tree and tree are equivalent trees). A corresponding agent

apply(flatten(3), [X,Y,Z]) :-
  → flatten(X, Y, Z).
apply(flatten(3,X), [Y,Z]) :-
  → flatten(X, Y, Z).
apply(flatten(3,X,Y), [Z]) :-
  → flatten(X, Y, Z).
apply(flatten(3,X,Y,Z), []) :-
  → flatten(X, Y, Z).

is also defined. In practice, it is convenient to regard apply as being defined implicitly for all definitions in a program, which is also easily achieved.

This functionality may now be used as in functional programs as follows. We define an agent map/3, which maps a list to another list.

map(P, [], Ys) :-
  → Ys = [].
map(P, [X|Xs], Ys0) :-
  → Ys0 = [Y|Ys],
    apply(P, [X, Y]),
    map(P, Xs, Ys).

and may then call it with, e.g., map(append(3,[a]), [[b],[c]], Ys) and get the result Ys = [[a,b],[a,c]].

Although by no means necessary, expressions corresponding to lambda expressions can also be introduced. Let an expression

(X1, …, Xk)\A

where A is a statement with free variables Y1, …, Ym, stand for a term

p((m+k), Y1, …, Ym)

where p/(m+k) is a new agent defined as

p(Y1, …, Ym, X1, …, Xk) := A.

We may now write, e.g., map((X,Y)\append(X, Z, Y), [[b],[c]], Ys) and get the result Ys = [[b|Z],[c|Z]]. Finally, the syntactic gap can be closed even further by introducing the notation

P(X1, …, Xk)

standing for

apply(P, [X1, …, Xk])

Obviously, the terms corresponding to functional closures may be given more efficient representations in an implementation.

#### 3.4.2 Relations

The relational paradigm is known from logic programming as well as from database query languages. Most prominent of logic programming languages is Prolog (see, e.g, [Clocksin and Mellish 1987; Sterling and Shapiro 1986; O'Keefe 1990]), which is entirely based on the relational paradigm. A large number of powerful programming techniques have been developed. Prolog and its derivatives are used for data and knowledge base applications, constraint satisfaction, and general symbolic processing. AKL supports Prolog-style programming. This relation is discussed in Section 8.1.

Characteristic of the relational paradigm is the idea that programs interpreted as defining relations should be capable of answering queries involving these relations. Thus, if a parent relation is defined, the program should be able to produce all parents for given children and all children for given parents, enumerate all parents and corresponding children, and verify given parents and children.

The following definition clearly satisfies this condition.

parent(sverker, adam).
parent(kia, adam).
parent(sverker, axel).
parent(kia, axel).
parent(jan_christer, sverker).
parent(hillevi, sverker).

It is also satisfied by any non-recursive AKL program that does not use conditional choice, committed choice, or bagof.

Maybe less intuitive, but just as appealing, is the following: a simple parser of a fragment of the English language. The creation of a parse-tree is omitted.

s(S0, S) := np(S0, S1), vp(S1, S).
np(S0, S) := article(S0, S1), noun(S1, S).
article([a|S], S).
article([the|S], S).
noun([dog|S], S).
noun([cat|S], S).
vp(S0, S) := intransitive_verb(S0, S).
intransitive_verb([sleeps|S], S).
intransitive_verb([eats|S], S).

The two arguments of each atom represent a string of tokens to be parsed as the difference between the first and the second argument. The following is a sample execution.

s([a, dog, sleeps], S)
np([a, dog, sleeps], S2), vp(S2, S)
article([a, dog, sleeps], S1), noun(S1, S2), vp(S2, S)
noun([dog, sleeps], S2), vp(S2, S)
vp([sleeps], S)
intransitive_verb([sleeps], S)
S = []

The relation defined by s is

s([a, dog, sleeps|S], S)     s([a, dog, eats|S], S)
s([a, cat, sleeps|S], S)     s([a, cat, eats|S], S)
s([the, dog, sleeps|S], S)   s([the, dog, eats|S], S)
s([the, cat, sleeps|S], S)   s([the, cat, eats|S], S)

for all S, and will be generated as (don't know nondeterministic) alternative results from

s(S0, S)

The idea of a pair of arguments representing the difference between lists is important enough to warrant syntactic support in Prolog, the DCG syntax, which allows the above definitions to be rendered as follows.

s --> np, vp.
np --> article, noun.
article --> [a].
article --> [the].

and so on.

The example is naive, since real examples would be unwieldy, but the state of the art is well advanced, and the literature on *unification grammars* based on the above simple idea is rich and flourishing.

### 3.5 CONSTRAINT PROGRAMMING

Many interesting problems in computer science and neighbouring areas can be formulated as constraint satisfaction problems (CSPs). To these belong, for example, Boolean satisfiability, graph colouring, and a number of logical puzzles (a couple of which will be used as examples). Other, more application oriented, problems can usually be mapped to a standard problem, e.g., register allocation to graph colouring. In general, these problems are NP-complete; any known general algorithm will require exponential time in the worst case. Our task is to write programs that perform well in as many cases as possible.

A CSP can be defined in the following way. A (finite) *constraint satisfaction problem* is given by a sequence of variables X1, …, Xn; a corresponding sequence of (finite) domains of values D1, …, Dn; and a set of constraints c(Xi1, …, Xik). A solution is an assignment of values to the variables, from their corresponding domains, which satisfies all the constraints.

For our purposes, a constraint can be regarded as a logical formula, where satisfaction corresponds to the usual logical notion, but formalism will not be pressed here. Instead, AKL programs are used to describe CSPs, and their intuitive logical reading provides us with the corresponding constraints. Each agent is regarded as a (user-defined) constraint, and will be referred to as such. These agents are typically don't know nondeterministic, and those assignments for which the composition of these agents does not fail are the solutions of the CSP.

The example to be used in this section is the *n-queens problem*: how to place *n* queens on an *n* by *n* chess board in such a way that no queen threatens another. The problem is very well known, and no new algorithm will be presented. The novelty, compared to solutions in conventional languages, lies in the way the algorithm is expressed. The technique used is due to Saraswat [1987b], and was also used by Bahgat and Gregory [1989].

Each square of the board is a variable V, which takes the value 0 (meaning that there is no queen on the square) or 1 (meaning that there is a queen on the square).

The basic constraint is that there may be *at most one* queen in each row, column, and diagonal. Given that n queens are to be placed on an n by n board, a derived constraint, which we will use, is that there must be *exactly one* queen in each row and column. Note that the exactly-one constraint can be decomposed into an at-least-one and an at-most-one constraint. We now proceed to define these constraints in terms of smaller components. The problem is not only to express the constraint, which is easy, but to express it in such a way that an appropriate level of propagation will occur, which will reduce the search space dramatically.

The *at-most-one* constraint can be expressed in terms of the following agent.

xcell(1, N, N).
xcell(0, _, _).

Note that this agent is determinate (Section 2.5) if the first argument is known, or if the last two arguments are known and different.

For a sequence of squares V1 to Vk, we can now express that at most one of these squares is 1 using the xcell agent as follows.

xcell(V1, N, 1),
xcell(V2, N, 2),
…,
xcell(Vk, N, k)

If more than one Vi is 1, the variable N will be bound to two different numbers, and the constraint will fail. Let us call this constraint at_most_one(V1, …,Vk), thus avoiding the overhead of having to write a program to create it.

An at_most_one constraint will clearly only have solutions where at most one square is given the value 1, but note also the following propagation effects. If one of the Vi is given the value 1, its associated xcell agent becomes determinate, and can therefore be reduced. When it is reduced, N is given the value i, and the other xcell agents become determinate, and can be reduced, giving their variables the value 0.

The *at-least-one* constraint can be expressed in terms of the following agent.

ycell(1, _, _).
ycell(0, S, S).

Note that this agent too is determinate if the first argument is known, or if the other two arguments are known and different.

For a sequence of squares V1 to Vk, we can express that at least one of these squares is 1 using the ycell agent as follows.

S0 = true,
ycell(V1, S0, S1),
ycell(V2, S1, S2),
…,
ycell(Vk, Sk-1, Sk),
Sk = false

If all the squares are 0, a chain of equality constraints, S0 = S1, S1 = S2, …, will connect the symbols 'true' and 'false', and the constraint will fail. This constraint we call at_least_one(V1, …, Vk).

Again note the propagation effects. If a variable is given the value 0, then its associated ycell agent becomes determinate. When it is reduced, its second and third arguments are unified. If all variables but one are 0, the second argument of the remaining ycell agent will be 'true' and its third argument will be 'false', and it will therefore be determinate. When it is reduced, its first argument will be given the value 1.

Thus, not only will these constraints avoid the undesirable cases, but they will also detect cases where information can be propagated. When no agent is determinate, and therefore no information can be propagated, alternative assignments for variables will be explored by trying alternatives for the xcell and ycell agents.

A program solving the n-queens problem can now be expressed as follows.

• For each column, row, and diagonal, consisting of a sequence of variables V1, …, Vk, the constraint at_most_one(V1, …, Vk) has to be satisfied.

• For each column and row, consisting of a sequence of variables V1, …, Vn, the constraint at_least_one(V1, …, Vn) has to be satisfied.

• The composition of these constraints is the program.

Note that when information is propagated, this will affect other agents, making them determinate. This will often lead to new propagation. One such case is illustrated in Figure 3.4.

#### Figure 3.4. A state when solving the 4-queens problem

```
| 1 | 0   | 0   | 0   |
| 0 | 0   | V23 | V24 |
| 0 | V32 | 0   | V34 |
| 0 | V42 | V43 | 0   |
```

The above grid represents the board, and in each square is written the variable representing it, or its value if it has one. We will now trace the steps leading to the above state. Initially, all variables are unconstrained, and all the constraints have been created. Let us now assume that the topmost leftmost variable (V11) is given the value 1. It appears in the row V11 to V14, in the column V11 to V41, and in the diagonal V11 to V44. Each of these is governed by an at_most_one constraint. By giving one variable the value 1, the others will be assigned the value 0 by propagation.

A second case of propagation is that in Figure 3.5 (next page), where V12 and V24 are assumed to contain queens, and propagation of the above kind has taken place.

Here we examine the propagation that this state will lead to. Notice that in row 3, all variables but V31 have been given the value 0. This triggers the at_least_one constraint governing this row, giving the last variable the value 1, which in turn gives the variables in the same row, column, or diagonal (only V41) the value 0. Finally, V43 is given the value 1 by reasoning as above.

#### Figure 3.5. Another state when solving the 4-queens problem

```
| 0   | 1 | 0   | 0   |
| 0   | 0 | 0   | 1   |
| V31 | 0 | 0   | 0   |
| V41 | 0 | V43 | 0   |
```

In comparison, n-queens programs written using *finite domain constraints* do not exploit the fact that both rows and columns should contain exactly one queen (e.g., [Van Hentenryck 1989; Carlson, Haridi, and Janson 1994]). They are, however, much faster since propagation is performed by specialised machinery.

A better solution can be obtained if all the xcell and ycell agents are ordered so that those governing variables closer to the centre of the board come before those governing variables further out. If at some step alternatives have to be tried for an agent, values will be guessed for variables at the centre first. This is a good heuristic for the n-queens problem.

### 3.6 INTEGRATION

So far, the different paradigms have been presented one at a time, and it is quite possibly by no means apparent in what relation they stand to each other. In particular the relational and the constraint satisfaction paradigms have no apparent connection to the process paradigm. Here, this apparent dichotomy will be bridged, by showing how a process-oriented application based on the solver for the n-queens problems could be structured.

The basic techniques for interaction with the environment (e.g., files and the user) are discussed first, and then an overall program structure is introduced.

#### 3.6.1 Interoperability

The idea underlying interoperability is that an AKL agent sees itself as living in a world of AKL agents. The user, files, other programs, all are viewed as AKL agents. If they have a state, e.g., file contents, they are closer to objects, such as those discussed in Section 3.3. It is up to the AKL implementation to provide this view, which is inherited from the concurrent logic programming languages.

A program takes as parameter a port to the "operating system" agent, which provides further access to the functionality and resources it controls. An interface to foreign procedures adds glue code that provides the necessary synchronisation, and views of mutable data structures as ports to agents.

The details of this form of interoperability have not yet been worked out. The examples use imaginary, although realistic, primitives, as in the following.

main(OS) :=
  send(create_window(W, [xwidth=100, xheight=100]), OS),
  send(draw_text(10, 10, 'Hello, world!'), W).

Here it is assumed that the agent main is supplied with the "operating system" port OS when called. It provides window creation, an operation that returns a port to the window agent, which provides text drawing, and so on.

For some kinds of interoperability, a consistent view of don't know nondeterminism can be implemented. For example, a subprogram without internal state, such as a numerical library written in C, does not mind if its agents are copied during the course of a computation. For particular purposes, it is even possible to copy windows and similar "internal" objects. But the real world does not support don't know nondeterminism. It would hardly be possible to copy an agent that models the actual physical file system; nor would it be possible to copy an agent that models communication with another computer.

The only solution is to regard this kind of incompleteness as acceptable, and either let attempts to copy such unwieldy agents induce a run-time error, or give statements a "type" which is checked at compile-time, and which shows whether a statement can possibly employ don't know nondeterminism.

#### 3.6.2 Encapsulation

To avoid unwanted interaction between don't know nondeterministic and process-oriented parts of a program, the nondeterministic part can be *encapsulated* in a statement that hides nondeterminism. Nondeterminism is encapsulated in the guard of a conditional or committed choice and in bagof.

When encapsulated in the guard of a conditional or committed choice, a nondeterministic computation will eventually be pruned. In a conditional choice, the first solution is chosen. In a committed choice, any solution may be chosen.

When encapsulated in bagof, all solutions will be collected in a list.

More flexible forms of encapsulation can be based on the notion of *engines*. An engine is conceptually an AKL interpreter. It is situated in a server process. A client may ask the engine to execute programs, and, depending on the form of engine, it may interact with the engine in almost any way conceivable, inspecting and controlling the resulting computation. A full treatment of engines for AKL is future work.

#### 3.6.3 A Program Structure

Responsibilities can be assigned to the components of a program as illustrated by Figure 3.6. The view presents output on appropriate devices. The controller interprets input and acts as a relay between the model and the view. The model is where the actual computation takes place, and this is also where don't know nondeterminism should be encapsulated.

#### Figure 3.6. The basic program structure

```
┌──────┐   ┌────────────┐   ┌───────┐
│ View │←──│ Controller │←──│ Model │
└──────┘   └────────────┘   └───────┘
    ↓           ↑               ↑
    └───────────┼───────────────┘
                │
    foreign-    OS    message-passing
    function          interfaces
    interfaces
                │
           ┌────┴────┐
           │   OS    │
           └─────────┘
```

To exemplify the above, we apply it to an n-queens application. We assume the existence of a don't know nondeterministic n-queens agent

n_queens(N, Q) := … .

which returns different assignments Q to the squares of an N by N chess board. It is easily defined by adding code for creation of constraints for different length sequences, and code for creating sequences of variables corresponding to rows, columns, and diagonals on the chess board. No space will be wasted on this trivial task here. We proceed to the program structure with which to support the application.

main(P) :=
  initialise(P, W, E),
  view(V, W),
  controller(E, M, S, V),
  model(M, S).

The initialise agent creates a window accepting requests on stream W and delivering events on stream E. The view agent presents whatever it is told to by the controller on the window using stream W. The model delivers solutions on the stream S to the n-queens problems submitted on stream M. The controller is driven by the events coming in on E. It submits problems to the model on stream M and receives solutions on stream S. It then sends solutions to the view agent on V for displaying.

Let us here ignore the implementation of the initialise, view, and controller agents. The interesting part is how the don't know nondeterminism is encapsulated in the model agent. We assume that we are satisfied with being able to get either one or all solutions from the particular instance of the n-queens problem, or getting the reply that there are no solutions (for N = 2 or N = 3).

model([], S) :-
  → S = [].
model([all(N)|M], S) :-
  → bagof(Q, n_queens(N, Q), Sols),
    S = [all(Sols)|S1],
    model(M, S1).
model([one(N)|M], S) :-
  → ( Q : n_queens(N, Q)
    → S = [one(Q)|S1]
    ; S = [none|S1] ),
    model(M, S1).

As described above, don't know nondeterminism is encapsulated in bagof and conditional choice statements.
