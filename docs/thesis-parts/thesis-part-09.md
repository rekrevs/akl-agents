# Thesis Part 09

## AN EXECUTION MODEL (continued)

### 5.6 EXECUTION STATES AND TRANSITIONS

An execution state is a tuple

(γ, sus, alst, clst, ctx)

where γ is a configuration, sus is a set of suspensions, alst is an and-list, clst is a choice-list, and ctx is a context list. The symbol ζ stands for execution states. The execution model is a structure 〈E, ⇒〉, where E is the set of execution states and ⇒ ⊆ E × E is a transition relation on execution states.

The transition relation ⇒ is defined by the derivation rules that follow. When a transition involves the application of one or several computation rules, this is explicitly stated, and also clearly reflected in the transition on the configuration part of the execution states involved. The context χ will be the same as in the corresponding computation rule, except for the choice splitting rule, which uses the contexts of Section 4.5 (Nondeterminism and Stability).

With each transition is associated a set of labels that are new in the transition, which will be used in Section 5.8.

#### 5.6.1 Statement Tasks

**Constraint Atom Success**

(χ[ l::and(R, i::A, S)σ
V ], sus, s(i).alst, clst, ctx)
⇒
(χ[ l::and(R, S)A∧σ
V ], sus′, w(l, j1)…w(l, jm).alst, clst, ctx)    (1)

The constraint atom rule is applied if env(χ) and A∧σ are compatible.

The set sus′ is sus ∪ {v1.l, …, vn.l}, where {v1, …, vn} is a sufficient set of variables for suspension of ∃V(σ∧A) in an environment env(χ). The set {j1, …, jm} consists of all and-boxes in l suspended on variables in a sufficient set for waking when adding A to env(χ)∧σ. (Both n and m may be 0.)

**Constraint Atom Failure**

(χ[ j::and(R, i::A, S)σ
V ], sus, s(i).alst, clst, ctx)
⇒
(χ[ k::fail ], sus, a(k), clst, ctx)    (2)

The constraint atom rule and the environment synchronisation rule are applied if env(χ) and A∧σ are incompatible. The label k is new.

**Program Atom Execution**

(χ[ i::A ], sus, s(i).alst, clst, ctx)
⇒
(χ[ j::B ], sus, s(j).alst, clst, ctx)    (3)

The program atom rule is applied, where B is as in the rule. An s (statement) task will execute the body. The label j is new.

**Composition Execution**

(χ[ and(R, i::(A, B), S)σ
V ], sus, s(i).alst, clst, ctx)
⇒
(χ[ and(R, j::A, k::B, S)σ
V ], sus, s(j).s(k).alst, clst, ctx)    (4)

The composition rule is applied, splitting a composition into its component statements. Two s (statement) tasks will execute the components. The labels j and k are new.

**Hiding Execution**

(χ[ and(R, i::(U : A), S)σ
W ], sus, s(i).alst, clst, ctx)
⇒
(χ[ and(R, j::B, S)σ
V∪W ], sus, s(j).alst, clst, ctx)    (5)

The hiding rule is applied, where V and B are as in the rule. An s (statement) task will execute its component. The label j is new.

**Choice Execution**

(γ, sus, s(i).alst, clst, ctx)
⇒
(γ′, sus, ε, c(j1)…c(jn).o(k), (alst, clst).ctx)    (6)

The choice rule is applied, where

γ = χ[ i::(U1 : A1 % B1 ; … ; Un : An % Bn) ]

γ′ = χ[ k::choice((and(j1::A′1)true
V1 % B′1), …, (and(jn::A′n)true
Vn % B′n)) ]

and A′i, B′i, and Vi are as in the rule

A number of c (clause) tasks will execute the clauses, and an o (or) task will deal with promotion, failure, and suspension. The labels j1, …, jn, and k are new. The and-boxes introduced are also given new labels.

**Aggregate Execution**

(χ[ i::aggregate(u, A, v) ], sus, s(i).alst, clst, ctx)
⇒
(χ[ aggregate(w, j::or(and(k::B)true{w} ), v) ], sus, ε, c(k).o(j), (alst, clst).ctx)    (7)

The aggregate rule is applied, where w and B are as in the rule. A c (clause) task will execute the goal, and an o (or) task will deal with suspension and the unit rule. The labels j and k are new. The aggregate box and the and-box are also given new labels.

#### 5.6.2 Clause Tasks

**Clause Execution**

(χ[ i::and(j::A)true
V ], sus, ε, c(j).clst, ctx)
⇒
(χ[ i::and(j::A)true
V ], sus, s(j).a(i), clst, ctx)    (8)

Whether the first (or single) alternative, or one tried as a result of failure or suspension of preceding guards, this rule initiates execution in an and-box. The c (clause) task on the choice-list is replaced by two tasks on the and-list: (1) an s (statement) task for the statement in the and-box and (2) an a (and-box) task to deal with guards, aggregates, and the failure or suspension of an and-box.

#### 5.6.3 And-Box Tasks

**Promotion Execution**

(χ[ l::and(R, choice(i::σV % B), S)θ
W ], sus, a(i), clst, (alst′, clst′).ctx)
⇒
(χ[ l::and(R, j::B, S)σ∧θ
V∪W ], sus′, w(l, j1)…w(l, jn).s(j).alst′, clst′, ctx)    (9)

The promotion rule is applied. If % is '→' or '|' it is required that σ is quiet with respect to θ ∧ env(χ) and V.

The set sus′ is sus where suspensions v.i are replaced by suspensions v.l. The set {j1, …, jn} consists of all and-boxes in l suspended on variables in a sufficient set for waking when adding σ to env(χ)∧θ. The label j is new.

**Condition Execution**

(χ[ j::choice(R, i::σV → B, S) ], sus, a(i), clst, ctx)
⇒
(χ[ j::choice(R, i::σV → B) ], sus, a(i), o(j), ctx)    (10)

The condition rule is applied if S is non-empty and σ is quiet with respect to env(χ) and V.

**Commit Execution**

(χ[ j::choice(R, i::σV | B, S) ], sus, a(i), clst, ctx)
⇒
(χ[ j::choice(i::σV | B) ], sus, a(i), o(j), ctx)    (11)

The condition rule is applied if at least one of R or S is non-empty and σ is quiet with respect to env(χ) and V.

**Collect Execution**

(γ, sus, a(i), clst, (alst′, clst′).ctx)
⇒
(γ′, sus, ε, clst, (s(j).alst′, clst′).ctx)    (12)

The collect rule is applied if σ is quiet with respect to env(χ) and V, where

γ = χ[ k::and(R1, aggregate(u, or(S1, i::σV, S2), v), R2)θ
W ]

γ′ = χ[ k::and(R1, aggregate(u, or(S1, S2), v'), j::collect(u', v', v), R2)σ∧θ
V∪W ]

Observe that since the choice splitting rule is augmented with variable renaming, there is no need for renaming in the collection step. The variable u′ is the copy corresponding to u in V, and v′ does not occur in γ.

An s (statement) task will execute the collect operation. It is entered in the saved context, to be executed upon completion or suspension of the aggregate. The constraints are promoted immediately. No waking can occur since they are quiet. Suspensions referring to the and-box i need not be promoted. The label j is new.

**Guard Failure Execution**

(χ[ choice(R, i::fail % B, S) ], sus, a(i), clst, ctx)
⇒
(χ[ choice(R, S) ], sus, ε, clst, ctx)    (13)

The guard failure rule is applied.

**Failure in Or Execution**

(χ[ or(R, i::fail, T) ], sus, a(i), clst, ctx)
⇒
(χ[ or(R, T) ], sus, ε, clst, ctx)    (14)

The or-flattening rule is applied with S as the empty sequence.

**Stable And-box Detection**

(χ[ i::G ], sus, a(i), clst, ctx)
⇒
(χ[ i::G ], sus, ε, cs(i).clst, ctx)    (15)

if no suspensions in sus refer to the and-box i or and-boxes in i, i.e., i is stable, and the preceding and-box task rules (9–14) are not applicable.

**And-Box Suspension**

(χ[ i::G ], sus, a(i), clst, ctx)
⇒
(χ[ i::G ], sus, ε, clst, ctx)    (16)

if the preceding and-box task rules (9–15) are not applicable.

#### 5.6.4 Or-Box Tasks

**Determinacy Detection**

(χ[ i::choice(j::σV % B) ], sus, ε, o(i), ctx)
⇒
(χ[ i::choice(j::σV % B) ], sus, a(j), o(i), ctx)    (17)

if % is ? or if σ is quiet with respect to env(χ) and V.

Determinacy of an or-box (due to failure of siblings) is detected. The single guard is re-entered, to perform promotion. No waking is possible since the and-box is solved.

**Goal Failure**

(χ[ and(R, i::choice(), S)σ
V ], sus, ε, o(i), (alst′, clst′).ctx)
⇒
(χ[ j::fail ], sus, a(j), clst′, ctx)    (18)

The goal failure rule is applied. The label j is new.

**Unit Execution**

(χ[ aggregate(u, i::fail, v) ], sus, ε, o(i), (alst′, clst′).ctx)
⇒
(χ[ j::unit(v) ], sus, s(j).alst′, clst′, ctx)    (19)

The unit rule is applied. The s (statement) task will execute the unit operation. The label j is new.

**Choice-Box Suspension**

(χ[ i::choice(R) ], sus, ε, o(i), (alst′, clst′).ctx)
⇒
(χ[ i::choice(R) ], sus, alst′, clst′, ctx)    (20)

unless rules 17 or 18 are applicable.

**Or-Box Suspension**

(χ[ i::or(R) ], sus, ε, o(i), (alst′, clst′).ctx)
⇒
(χ[ i::or(R) ], sus, alst′, clst′, ctx)    (21)

unless rule 19 is applicable.

#### 5.6.5 Choice Splitting Tasks

As opposed to the other execution rules, in this section the contexts (χ and χ′) are named as in Section 4.5 (Nondeterminism and Stability) instead of as in the corresponding computation rule (choice splitting).

```
         copied and-box
           .....
          /     \
         /   χ′  \
        /         \
       /     G     \
      /             \
     χ
```

Figure 5.3. Contexts and goals involved in choice splitting

**Choice Splitting Execution**

(χ[ i::G ], sus, ε, cs(i).clst, ctx)
⇒
(γ, sus′, ε, clst′, ctx′)    (22)

Since G is known to be stable, the choice splitting rule may be applied if G is of the form χ′[ and(S1, choice(T1, T2), S2)σ
V ], where T1 = (j::θw ? A). (There is no need to commit to a particular choice of candidate.) If choice splitting is not applicable, let γ = χ[ i::G ], sus′ = sus, clst′ = clst, and ctx′ = ctx, which is equivalent to and-box suspension.

Due to the handling of suspensions, the goals in the new branch need not only be duplicated, as suggested by the computation rule, but also copied, meaning that they will be given new labels and that local variables will be renamed. The copy we call G1 and the copy of the candidate is labelled j′. All labels of goals in G1 are new.

G1 = and(S1′, k1::choice(j′::θ′w′ ? A′), S2′)σ′
V′

The right branch retains the old labels.

G2 = and(S1, k2::choice(T2), S2)σ
V

Pertinent suspensions are duplicated.

sus′ = sus ∪ sus1 ∪ sus2
sus1 = { v.j | v.i ∈ sus, v external to G2, j copy of i }
sus2 = { v.j | u.i ∈ sus, u local to G2, j copy of i, v renaming of u }

If χ[χ′] = χ′′[ choice(R1, λ % B, R2) ], then splitting is applied in the scope of a choice-box. The guard distribution rule and guard failure rules are applied, removing immediately the new or-box created by splitting. The body B is copied, renaming variables V to corresponding variables in V′.

γ = χ′′[ l::choice(R1, G1 % B′, G2 % B, R2) ]

The choice-box in which splitting is performed is labelled l.

If χ[χ′] = χ′′[ or(R1, λ, R2) ], then splitting is applied in the scope of an or-box. The or-flattening rule is applied, removing the new or-box. Observe that the interaction with aggregates is explained with rule 12 (collect execution).

γ = χ′′[ l::or(R1, G1, G2, R2) ]

The or-box in which splitting is performed is labelled l.

If χ′ = λ then choice splitting was performed with respect to the stable and-box. We promote the copied solution and remember to retry choice splitting. If the remaining choice-box is promotable, we promote it instead.

ctx′ = ctx
if T2 = (θW ? A′′) then clst′ = p(k1).p(k2).clst
else clst′ = p(k1).cs(i).clst

Otherwise, choice splitting was performed within the stable and-box. Again, we should promote the copied solution, from the point of view of the parent choice- or or-box, but choice splitting should not be retried at that point, since the boxes are not stable. However, if the remaining choice-box is promotable, it has to be promoted.

Let i0, j0, …, in, jn be the labels of and-boxes (is) and choice- or or-boxes (js) in the path from the stable and-box labelled i = i0 to the choice- or or-box in which splitting is performed labelled l = jn.

ctx′ = (a(in), o(jn-1)). … .(a(i1), o(j0)).(a(i0), clst).ctx
if T2 = (θW ? A′′) then clst′ = p(k1).p(k2).o(l)
else clst′ = p(k1).o(l)

Stability guarantees that no failure will occur and no suspensions need be waked by moving down.

#### 5.6.6 Choice Promotion Tasks

**Promote after Splitting**

(χ[ j::and(R, i::choice(k::G ? B), S)σ
V ], sus, ε, p(i).clst, ctx)
⇒
(χ[ j::and(R, i::choice(k::G ? B), S)σ
V ], sus, a(j), o(i), ctx′)    (23)

where

ctx′ = (a(j), clst).ctx

which makes it ready for rule 9 (promotion execution). No waking is possible, since we are in a goal which was stable before choice splitting, and if we are in a aggregate, collect operations have not yet been executed.

#### 5.6.7 Wake-up Tasks

**Woken Up**

(γ, sus, w(i, j).alst, clst, ctx)
⇒
(γ, sus, alst, clst, ctx)    (24)

if i = j or j is the label of an and-box dead in γ.

**Wake Up**

(χ[ i::G ], sus, w(i, j).alst, clst, ctx)
⇒
(χ[ i::G ], sus, in(k).w(k, j).a(k), o(l), (alst, clst).ctx)    (25)

if i ≠ j and j is the label of an and-box live in γ, where

G = and(R1, l::choice(S1, G′ % B, S2), R2)σ
V

or G = and(R1, aggregate(u, l::or(S1, G′, S2), v), R2)σ
V

and G′ = χ′[j::G′′] = k::and(S)θ
W

**Installation Success**

(χ[ i::and(R)σ
V ], sus, in(i).alst, clst, ctx)
⇒
(χ[ i::and(R)σ
V ], sus′, w(i, j1)…w(i, jm).alst, clst, ctx)    (26)

if env(χ) and σ are compatible.

The set sus′ is sus ∪ {v1.i, …, vn.i}, where {v1, …, vn} is a sufficient set of variables for suspension of ∃Vσ in an environment env(χ). The set {j1, …, jm} consists of all and-boxes in i suspended on variables in a sufficient set for waking when adding σ to env(χ). (It is only necessary to wake at this point if the suspension-waking scheme requires multistage waking, as for the optimised treatment of rational trees.)

**Installation Failure**

(χ[ i::and(R)σ
V ], sus, in(i).alst, clst, ctx)
⇒
(χ[ j::fail ], sus, a(j), clst, ctx)    (27)

The environment synchronisation rule is applied if env(χ) and σ are incompatible. The label j is new.

### 5.7 EXECUTIONS

A partial execution is a finite or infinite sequence of execution states

ζ0 ⇒ ζ1 ⇒ ζ2 ⇒ …

in which labels that are new in the transition to ζi do not occur as labels of goals in any preceding state ζj (j < i).

A execution state ζ which satisfies ¬∃ζ'. ζ ⇒ ζ' is terminal.

An initial execution state is of the form

(or(and(j::A)true
V ), sus, ε, c(j), ε)

where sus is an empty set of suspensions.

A final execution state is of the form

(γ, sus, ε, ε, ε)

where γ is a final configuration. A terminal execution state that is not final is stuck.

An execution is a partial execution beginning with an initial execution state and, if finite, ending with a terminal execution state.

By the goal to which a computation rule can be applied, we mean (1) for statement rules, the statement itself, (2) for the pruning and collect rules, the solved andbox, (3) for the promotion rule, the solved and-box or the choice-box, (4) for the environment failure rule, the and-box replaced by fail, (5) for the goal failure rule, the empty choice-box, (6) for the guard failure rule, the failed guard, (7) for the choice splitting rule, the stable and-box, (8) for the unit rule, the empty or-box corresponding to fail, and (9) for the or-flattening rule, in the case of flattening an empty or-box, the empty or-box.

Below we assume that the scheme for suspension and waking is complete.

LEMMA 1. In all execution states that do not contain in, all subgoals of the configuration to which a computation rule can be applied are referred to by tasks.

PROOF. This is shown by induction on execution steps.

We first observe, by inspecting all rules, that in all task lists, tasks refers to subgoals of the box referred to by the a or o task that end them. We also observe, by inspecting the rules that introduce multiple choice-tasks (6 and 22), that tasks in choice-lists refer to subgoals of the pertaining or- or choice-box in leftto-right order, and that tasks are processed in this order.

The initial state contains a statement to which a computation rule can be applied, which is referred to by a c task.

It can be argued for each execution rule, by tedious inspection, that the goals to which computation rules can be applied after a corresponding step are either given new tasks or are already referred to by tasks. The more interesting cases are described.

The constraint atom and installation execution rules can lead (1) to the incompatibility with its environment of the local store, (2) to the quietness or incompatibility of constraint stores in subgoals of the and-box, or (3) to the and-box being solved. Either (zero or more) w tasks are entered that refer to all andboxes for which constraint stores may have become quiet or incompatible, or an a task is left that refers to the failed and-box. In the first case, if the and-box is solved, it is referred to by the a task in the list. In the second case, the tasks removed refer to dead goals.

The condition and commit execution rules remove all siblings or siblings to the right of the solved and-box. This and-box was reached by what was the first task on the pertaining choice-list. Therefore, the other tasks, except o, refer to subgoals to the right, and may safely be removed in both cases.

The collect execution rule moves tasks in an unusual way, but adds an s task for the new statement, and there is already an o task for the or-box, in case it becomes empty.

The guard failure execution and failure-in-or execution rules remove goals, and have already o tasks for the choice- or or-box, in case they become determinate or empty.

The choice splitting execution rule can suspend due to the lack of a candidate. Otherwise, it makes promotion applicable to the copy of the candidate. Promotion may have become applicable in the right branch. If the copied and-box was stable, the right branch may still be stable. These possibilities are handled by tasks introduced.

The remaining rules rather trivially preserve the property. ♦

LEMMA 2. A terminal execution state in an execution is of the form

(γ, sus, ε, ε, ε)

where γ is a terminal configuration.

PROOF.

(1) All tasks have rules that apply in any execution state that may arise. In particular, the a and o tasks have suspension rules that pop the context stack. Thus, the task and context stacks are empty in a terminal state.

(2) If there are no tasks, γ is a terminal configuration, by Lemma 1. ♦

LEMMA 3. Only a finite number of transitions on an execution state are made between transitions on its configuration.

PROOF. The tasks s, c, a, o, cs, and p either lead to transitions, or are skipped by suspension rules and the like, and there is only a finite number of tasks to skip. Unless the configuration is changed by a transition, each in and w leads to a finite number of w and in, since the tree of and-boxes visited is finite. ♦

PROPOSITION (correctness). The sequence of configurations in an execution, in which subsequent equal configurations are deleted, forms a computation.

PROOF. An initial execution state contains an initial configuration. Transitions between execution states that modify the configuration do so by applying computation rules, which are implicitly propagated by the subgoal rule, and where we for copying make implicit use of a rule that allows us to rename the local variables of and-boxes. Terminal execution states contain terminal configurations by Lemma 2. Execution is guaranteed to make progress and either produce a finite or an infinite computation by Lemma 3. ♦

### 5.8 DISCUSSION

The execution model presented is not fair, in that a task can remain unattended indefinitely. For example,

p, a = b

where

p :- p.

will loop, not fail.

A fair execution model is arguably more appealing to the intuition. All agents are active, and have their own "virtual processor". It is easier to reason about some properties of programs. However, the intuition can also be misleading.

A common misunderstanding is that a producer and a consumer will execute in a, more or less, constant size execution state (if garbage collection simplification is performed). This is not so. The producer can produce at, say, twice the speed of the consumer. To guarantee constant size, a bounded buffer programming technique has to be used (see, e.g., [Shapiro 1987]).

Another misunderstanding is that fairness models the notion of background processes, for example, background re-pagination of a document in a word processor while it is being edited. Basic fairness, however, has nothing to say about the resources devoted to each task. Re-pagination might very well use 90% of the time, and slow down editing, or it might use 0.1%, and never catch up.

These misunderstandings are usually beginner's problems.

A problem of a quite different nature is that by stipulating fairness we also stipulate "breadth-first" search in programs employing don't know nondeterminism. This could be relaxed, by giving (unfair) priority to the leftmost alternative in don't know choice, conditional choice, and ordered bagof, since the
