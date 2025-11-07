# Thesis Part 10

## AN EXECUTION MODEL (continued)

difference cannot be observed. Such stipulations would be fundamental to the nature of the language.

A fair execution model for AKL is also considerably more complex. The investigation of AKL, and related languages, has not yet reached the point where it would be possible to argue conclusively for or against general fairness.

Meanwhile, it is obvious that, to be useful for, e.g., background processes, even a fair execution model has to be complemented with some form of metalevel control that allocates computational resources to different subcomputations. Such directives are available in the Oz language and system [Smolka et al 1994].

---

# CHAPTER 6

## AN ABSTRACT MACHINE

The abstract machine presented here is a refinement of the execution model, with some discrepancies related to waking. It specifies in more detail the representation of programs and execution states, and how tasks are executed on this representation. These representations are chosen to be suitable for implementation on a concrete machine, but are not discussed at the implementation level.

The abstract machine is particular to the constraint system of rational trees. The requirements of arbitrary constraint solvers are not sufficiently understood at this point to allow a fully general treatment. Carlson, Carlsson, and Diaz [1994] and Keisu [1994] have investigated particular constraint solvers—for finite domain constraints and rational tree constraints closed under all logical combinators.

A concrete implementation exists, the AGENTS system, that is based on a closely related abstract machine [Janson and Montelius 1992; Janson et al 1994]. The abstract machine of this presentation is for the most parts considerably simplified, but supports the eager waking behaviour of the execution model as well as the lazy waking of AGENTS 0.9. Some aspects of the concrete implementation are discussed.

No attempt has been made to describe an optimal machine. On the contrary, much has been sacrificed to keep the presentation simple. The purpose of this chapter is to introduce fundamental machinery for this particular style of abstract machine. Some important optimisations are described.

### 6.1 OVERVIEW

An execution state corresponds to a machine state as follows.

The (labelled) configuration is represented by and-nodes (corresponding to guarded goals with and-box guards or to and-boxes in or-boxes), choice-nodes (corresponding to choice-boxes, aggregate-boxes with or-boxes, or to the toplevel or-box), and-continuations (corresponding to statements), choice-continuations (corresponding to parts of choice-statements), variables, symbols, tree constructors, constraints, and instructions (representing statements)

The set of suspensions is represented by suspensions on variables and the trail.

The combination of and-list, choice-list, and context list is represented by the and-stack, the choice-stack, the context stack, and the current and-node register.

Agent definitions and their component statements are represented by instructions. The choice of instruction set is largely orthogonal to the rest of the abstract machine. An instruction set which is loosely based on the WAM is presented [Warren 1983], and some optimisations are suggested.

### 6.2 DATA OBJECTS

Objects are characterised by their types and attributes. All objects and some attributes of objects have unique references in a given machine state. For the representations of goals, these correspond to labels. It is assumed that there is a reference, called null, that is different from the reference of any object, and which may be used in the place of any reference. Objects reside in different data areas (which are described in Section 6.3). Occasionally, a reference will be referred to as the object and vice versa, and a reference to an attribute as a reference to the object, for the purpose of less verbose descriptions, and no confusion is expected from this relaxation.

#### 6.2.1 And-Nodes

An and-node has the attributes

• parent: a reference to a choice-node
• alternatives: a pair of references to alternatives attributes
• constraints: a reference to a constraint (list)
• goals: a pair of references to goals attributes
• continuation: a reference to an and-continuation
• forward: a reference to an and-node
• state: one of live-stable, live-unstable, or dead

The and-node is on a double-linked list of alternatives, one link of which is in its parent choice-node. It has to support arbitrary insertion and deletion, and is double-linked for simplicity. Of the pair, one is called left and the other right.

The constraint list is null until the and-node is suspended for the first time (if ever). It will then contain bindings on external variables.

The and-node contains a double-linked list of goals, one link of which is in the and-node itself. Like the list of alternatives, it has to support arbitrary insertion and deletion, and is double-linked for simplicity. It is initially empty, i.e., its links refer to itself.

```
Choice-Node
┌─────────────┐
│parent       │◄──────┐
├─────────────┤       │
│goals        │       │  And-Node        And-Node        And-Node
├─────────────┤       │  ┌─────────┐     ┌─────────┐     ┌─────────┐
│alternatives │◄──────┼──┤parent   │     │parent   │     │parent   │
└─────────────┘       │  ├─────────┤     ├─────────┤     ├─────────┤
                      └──┤altern...│◄───►│altern...│◄───►│altern...│◄─┐
                         ├─────────┤     ├─────────┤     ├─────────┤  │
                         │constr...│     │constr...│     │constr...│  │
                         ├─────────┤     ├─────────┤     ├─────────┤  │
                         │goals    │     │goals    │     │goals    │  │
                         ├─────────┤     ├─────────┤     ├─────────┤  │
                         │contin...│     │contin...│     │contin...│  │
                         ├─────────┤     ├─────────┤     ├─────────┤  │
                         │forward  │     │forward  │     │forward  │  │
                         ├─────────┤     ├─────────┤     ├─────────┤  │
                         │state    │     │state    │     │state    │  │
                         └─────────┘     └─────────┘     └─────────┘  │
                                                                       │
                                                                       └─┘
```

Figure 6.1. Tree of and- and choice-nodes

The continuation represents the guard operator and the body of a guarded goal, or serves as a special sentinel in and-nodes in the top-level and in aggregates. As the constraint list, it is null until the and-node is suspended.

Until promotion, the forward pointer is null. Then, it is set to refer to the andnode to which promotion took place.

The state of an and-node is either live or dead. The state of a live and-node is further subdivided into live-stable or live-unstable. In a node which is live-stable it is known that no constraint on external variables exists within the node. In a node which is live-unstable there might be such a constraint. An and-node is initially marked as stable. If a constraint is placed on a variable external to the and-node it is marked as unstable. An and-node that has been marked as unstable will never be marked as stable again. An and-node is marked as dead if it fails, is pruned, or is promoted.

#### 6.2.2 Choice-Nodes

A choice-node has the attributes

• parent: a reference to an and-node
• goals: a pair of references to goals attributes
• alternatives: a pair of references to alternatives attributes

In the root choice-node, the parent and-node is null.

The choice-node is in a double linked-list of goals, one link of which is in the parent and-node.

The choice-node contains a double linked-list of alternatives, one link of which is in the choice-node itself.

Figure 6.1 illustrates how and- and choice-nodes are connect via parent and double links. In the figure, a choice-node is the parent and it and the and-nodes are linked via an alternatives chain. The situation in which an and-node is the parent and the choice-nodes are linked via a goals chain is symmetric.

A node is in another node, if the latter can be reached from the former by following parent links. By the grandparent of a node is meant its parent's parent. By empty alternatives and goals, we mean that they contain no and-nodes or choice-nodes, respectively. An object in a double-linked list is known to be leftmost (right-most) if its left (right) reference refers to an attribute in its parent. Familiar notions such as inserting and unlinking objects will be used on singleand double-linked lists.

#### 6.2.3 Constraints and Variables

The abstract machine is specific to the constraint system of rational trees. There are three types of components in tree expressions: symbols, tree constructors, and variables. A tree is a reference to a tree component. In terms of the computation model, a tree is a variable which is constrained to be equal to the expression to which it refers.

A symbol has no attributes.

A tree constructor has the attributes

• functor: the name and arity
• arguments: a vector of trees

A variable has the attributes

• value: a tree
• state: one of bound or unbound
• home: a reference to an and-node
• suspensions: a reference to a suspension

In a new variable, suspensions is null and the state is unbound.

A suspension has the attributes

• next: a reference to a suspension (list)
• suspended: a reference to an and-node

The constraints in an and-node correspond to bindings to external variables.

A constraint has the attributes

• next: a reference to a constraint
• variable: a reference to a variable
• value: a tree

#### 6.2.4 And-Continuations

An and-continuation has the attributes

• continuation pointer: a reference to a sequence of instructions
• y-registers: a vector of trees

The instructions represent a statement of which the trees in the vector are the (constrained) free variables.

And-continuations serve a dual purpose, as the representation of a statement and as the representations of corresponding s (statement) tasks.

#### 6.2.5 Choice-Continuations

A choice-continuation has the attributes

• continuation pointer: a reference to a sequence of instructions
• a-registers: a vector of trees

The instructions represent unexplored guarded goals in a choice-box of which the trees in the vector are the (constrained) free variables.

Like and-continuations, choice-continuations serve a dual purpose, as the representation of guarded goals and as the representation of a corresponding sequence of c (clause) tasks.

#### 6.2.6 Trail Entries

A trail entry has the attribute

• variable: a reference to a variable

A trail-entry refers to an external variable which has been bound locally.

#### 6.2.7 Tasks

The tasks of the abstract machine correspond closely to the tasks of the execution model.

The s (statement) and a (and-box) tasks are represented by and-continuations, which also serve as representations of the pertaining statements. The w (wake) task is explicit also in the abstract machine.

The c (clause) tasks are represented by choice-continuations, which also serve as representations of the pertaining guarded goals. The o (or-box, here called no-choice), cs (choice splitting), and p (promote) tasks are explicit also in the abstract machine.

When regarded as tasks, and- and choice-continuations are referred to as tasks.

There is a need for new tasks related to waking—resume and restore insertion point. At the point of a call, the program counter and the x-registers represent a program atom that has to be made explicit when switching context. The insertion point corresponds to the label in the s tasks.

The no-choice task has no attributes.

The wake, choice splitting, and promote tasks have the attribute

• node: a reference to an and-node

The resume task has the attributes

• continuation pointer: a reference to a sequence of instructions
• a-registers: a vector of trees

The restore insertion point task has the attribute

• goals: a pair of references to goals attributes

#### 6.2.8 Contexts

A context has the attributes

• and: a reference to an and-task or and-continuation
• choice: a reference to a choice-task or choice-continuation
• trail: a reference to a trail entry

Contexts serve the purpose of the context list in the execution model, namely to organise tasks in and- and choice-node levels. The trail attribute is a part of the representation of the set of suspensions.

### 6.3 DATA AREAS AND REGISTERS

#### 6.3.1 Data Areas

The abstract machine has seven data areas: heap, trail stack, and-stack, choice stack, context stack, wake-up stack, and static store (Figure 6.2, next page).

The heap is an unordered collection of and-nodes, choice-nodes, tree constructors, variables, suspension lists, constraint lists, and and-continuations. New objects can be created on the heap and are then assigned unique references. Objects on the heap that are not referenced are removed by garbage collection (which is not discussed further).

A stack is a sequence of objects, regarded as extending upwards. New objects can be pushed on the stack and are then assigned unique references. For a given stack, the reference that would be assigned to a pushed object is known, and is called top. Any object in a stack can be removed, giving new references to all objects above it. When the top-most object is removed, it is pop'ed.

The trail-stack is a stack of trail entries. The and-stack is a stack of and-continuations and and-tasks. The choice-stack is a stack of choice-continuations and choice-tasks. The context-stack is a stack of contexts. The wake-up stack is a stack of references to and-nodes.

```
HEAP                                    TRAIL STACK
┌──────────────────┐
│and-nodes         │                   ┌──────────────┐      AND-STACK
│choice-nodes      │  TRAIL STACK      │              │
│tree constructors │                   │trail entries │      ┌──────────────────┐
│variables         │  ┌──────────────┐ │              │      │and-continuations │
│suspensions       │  │              │ └──────────────┘      │and-tasks         │
│constraints       │  │trail entries │                       └──────────────────┘
│and-continuations │  │              │  CONTEXT STACK
└──────────────────┘  └──────────────┘                       CHOICE-STACK
                                       ┌──────────┐
STATIC STORE                           │          │          ┌──────────────────┐
                      WAKE-UP STACK    │contexts  │          │choice-continu... │
┌──────────────┐                       │          │          │choice-tasks      │
│instructions  │     ┌──────────────┐  └──────────┘          └──────────────────┘
│symbols       │     │references to │
└──────────────┘     │and-nodes     │
                     └──────────────┘
```

Figure 6.2. Data areas of the abstract machine

The static store contains symbols and sequences of instructions.

Since the data area can often be inferred from the context, it will not always be explicitly named.

#### 6.3.2 Registers

Registers are implicit in the management of the data areas. We will speak of the current objects of stacks, meaning the objects immediately below their tops.

Other registers are

• the program counter: a reference to a sequence of instructions
• the current and-node: a reference to an and-node
• the insertion point: a reference to a goals attribute
• the x-registers: a vector of trees
• the current argument: a reference to a tree
• the unification mode: one of read or write

By the notation xi is meant the ith x-register, counting from 0 (in accordance with the tradition). It is assumed that all x-registers exist that are accessed by instructions currently in the static store.

By the notation yi is meant the ith element of the y-registers attribute of the current and-continuation, counting from 0. By the current choice-node we mean the parent of the current and-node.

#### 6.3.3 Initial States

In an initial state, the static store contains instructions and symbols for a program, as given by Section 6.6. All other data areas are empty. The current and-node is null. The instruction pointer refers to the code for the initial statement, as given by Section 6.6.7. The machine is thus ready to enter the instruction decoding state.

#### 6.3.4 Useful Concepts

An object in the and-stack, choice-stack, or trail stack is in the current context if it is equal to or above the object referred to by the corresponding attribute in the current context, or if the context stack is empty.

An and-node is solved when its goals attribute is empty and the program counter refers to a guard instruction.

An and-node is quiet when it is solved, the constraints list is empty, and there are no trail entries in the current context.

A variable is local if its home attribute, dereferenced, refers to the current andnode. The variable is external if the current and-node is in the and-node referred to. Otherwise the variable is unrelated (an insignificant case).

To dereference an and-node, if its forward attribute is non-null, dereference it, otherwise return the and-node.

An and-node is dead if it, dereferenced, has state dead or is in an and-node, the state of which is dead. Otherwise, it is live.

An and-node is determinate if it is the only alternative and the topmost element of the choice-stack is a no-choice task.

### 6.4 EXECUTION

During execution, the abstract machine moves between main states as illustrated by Figure 6.3 (next page). These are completely defined by

• contents of data areas
• values for pertaining registers

The abstract machine can be halted and restarted again at these points with no extra information. Other, here transient, states exist that could be made welldefined in an implementation with the aid of extra registers.

The registers needed to enter a state are

• for Decode Instructions, (potentially) all except unification mode
• for Fail, the current and-node
• for Back Up, the current and-node
• for Wake, (potentially) all except unification mode
• for Proceed, the current and-node, insertion pointer
• for Split?, the current and-node

The following sections describe these states and their pertaining actions.

```
                    Proceed ◄──────────────────────────────────────┐
                       ▲          resume or              install   │
                       │       and-continuation                    │
         ┌─────────────┼───────────────────┐        determinate    │
         │             │                   │        choice-node    │
         │             │ proceed           │                       │
copy &   │             │                   ▼                       │
install  │           Wake ◄────── Decode ──────► Fail             │
         │             ▲          Instructions    │                │
         │             │             │            │                │
         │             │         unify            │                │
    Split? ◄───────────┴─────────────┘            │                │
         │     stable at guard                    │      fail at no│
         │                                         │      choice    │
         │                                         │                │
         │        choice                           ▼                │
         │    continuation                                          │
         │         │                                                │
         │         ▼                                                │
         └────► Back Up ◄───────────────────────────────────────────┘
                                    promote or
              no                    suspend
          candidate                 at no choice
```

Figure 6.3. States of the abstract machine

#### 6.4.1 Instruction Decoding

To decode instructions, interpret the instruction referenced by the program counter (according to Section 6.5). Unless the instruction moves to another state, continue decoding instructions, and, unless the instruction changes the program counter, move to the next instruction.

#### 6.4.2 Unifying and Binding

To unify x and y perform the following. A set of pairs of references is used for unification of cyclic structures. It is assumed to be empty when unify is entered from instruction decoding. Dereference x and y. If y is a local variable, swap x and y, then select the first applicable case below.

• If x and y are identical references, do nothing.
• If x is a variable, bind x to y.
• If y is a variable, bind y to x.
• If x and y are tree constructors with the same functor, then, if x.y is found in the set of pairs, then do nothing, otherwise, add x.y to the set and unify each pair of corresponding arguments.
• Otherwise, fail.

To bind x to y, set the variable to state bound and store y in its value attribute, then select the applicable case below.

• If x is a local variable, unlink all suspensions on x and push references to the and-nodes on the wake stack.
• If x is an external variable, unlink all suspensions to and-nodes in the current and-node from x, and push corresponding references to the suspended and-nodes on the wake stack. (It is also appropriate to unlink suspensions referring to dead and-nodes.) Push a trail entry for x.

Note that this is one point where the abstract machine differs from the execution model. Waking does not occur at the point of adding constraints, since it is not known which x-registers are used. Instead, waking is delayed until processing the next non-constraint statement.

To dereference a tree x, if x is a bound variable, dereference its value, otherwise return x. To dereference a register x, dereference the contents of x, and store the result in x.

#### 6.4.3 Failing

To fail the current and-node, change its state to dead and unlink it from alternatives. Remove all trail entries in the current context, resetting their variables to unbound. Reset all variables in the constraint list to unbound. Remove all andtasks in the current context. Remove all references from the wake-up stack. If there remains a single solved and determinate sibling and-node, install it and proceed. Otherwise, back up.

#### 6.4.4 Suspending

To suspend the current and-node, unless it has a continuation, store the program counter in the continuation pointer of the current and-continuation, create a copy of it on the heap, making the copy the continuation of the current andnode, and pop it from the and-stack. Reset all variables in the constraint list to unbound. For each trail entry in the current context: Remove it and reset its variable to unbound. Create a constraint referring to the variable and its value. and add it to the current and-node. Suspend the and-node on the variable. If the dereferenced value is a variable, suspend the and-node also on that variable.

To suspend an and-node on a variable, add a suspension for this and-node to the variable and, for each and-node in the path from the current and-node to, but not including, the home and-node of the variable, change its state to unstable.

#### 6.4.5 Backing Up

To back up, examine the top object of the choice-stack, and do one of the following depending on its type.

• If it is a choice continuation, decode instructions starting from its continuation pointer.
• If it is a promote task, remove it, install (simply) the and-node referred to from the parent choice-node, and promote it.
• If it is a choice splitting task, remove it, set the current and-node to the node referred to by the task, and attempt choice splitting.
• If it is a no-choice task, remove the task, and remove the top context. If the parent of the current choice-node is null, terminate the abstract machine. Otherwise, if there are no alternatives, set the current and-node to the parent and fail. Otherwise, set the insertion point to the right sibling of the current choice-node, set the current and-node to the parent, and proceed.

Note that, to avoid testing if the parent of the choice-node is null, a special nochoice task can be used in the root node.

#### 6.4.6 Waking

To wake at call with N arguments, push a resume task with the program counter as continuation pointer and the N x-registers as a-registers. Push a restore insertion point task and insert it at the insertion point. Wake and proceed.

To wake at proceed, push a restore insertion point task and insert it at the insertion point. Wake and proceed.

To wake at a guard, store the program counter in the continuation pointer of the current and-continuation, wake, and proceed.

To wake, for each reference to an and-node on the wake stack, remove it and, if the and-node is live, push a corresponding wake task.

Note that pushing a resume or insertion point task only to discover that no live node could be waked can easily be avoided in an implementation.

#### 6.4.7 Promoting

To promote the current and-node, set the forward attribute to refer to the parent of the current choice-node, here called the target and-node. Unlink the current choice-node, setting the insertion point to its right sibling. Promote the trail and promote constraints. Set the current and-node to the target and-node. Remove the current context and remove the no-choice task. Decode instructions starting with the next instruction (following the current guard instruction).

To promote simply, do as above, but do not promote the trail and constraints, which are known to be empty.

To promote the trail, for each trail entry in the current context, select the first applicable case below.

• If the home of its variable is the target and-node, remove the trail entry. If suspensions on the variable refer to and-nodes in the target and-node, push references to these on the wake stack and unlink the suspensions. Unlink also all suspensions referring to dead nodes.
• If the dereferenced value of the variable is a variable, the home of which is the target and-node, bind this latter variable to the former after changing the state of the former to unbound. Remove the trail entry.
• Otherwise, do nothing.

To promote constraints, for each constraint in the current and-node, select the first applicable case below.

• If the home of its variable is the target and-node, then, if suspensions on the variable refer to and-nodes in the target and-node, push references to these on the wake stack and unlink the suspensions. If there are no such suspensions, do nothing.
• If the dereferenced value of its variable is a variable, the home of which is the target and-node, bind this latter variable to the former after changing the state of the former to unbound.
• Otherwise, move the constraint to the target and-node.

#### 6.4.8 Pruning

To prune alternatives, unlink all alternatives to the right and left of the current and-node and mark them as dead. Delete all choice-tasks in the current context and push a no-choice task.

To prune alternatives to the right, do as above, but unlink only to the right.

#### 6.4.9 Proceeding

To proceed, examine the top object of the and-stack, and do one of the following depending on its type.

• If it is an and-continuation, decode instructions starting from the continuation pointer.
• If it is a wake task, pop it. If it refers to a live and-node, dereference and install it. Proceed.
• If it is a restore insertion point task, pop it, unlink it, set the insertion point to the right of the goals attributes referred to, and proceed.
• If it is a resume task, pop it, move the a-registers to corresponding x-registers, and decode instructions starting from the continuation pointer.

#### 6.4.10 Installing

To install an and-node when proceeding, for each of the and-nodes in the path from, but not including, the current and-node to the and-node to be installed, enter its parent choice-node and then enter itself. This process may be interrupted by failing when entering and-nodes.

To install an and-node when failing, backing up, or collecting, do as above, but start by entering the first and-node.

To enter a choice-node, push a context referring to the tops of the pertinent stacks, then a no-choice task.

To enter an and-node, make it the current and-node and make its goals attribute the insertion point. For each constraint, select the first applicable case below.

• If its dereferenced variable is equal to its dereferenced value, unlink the constraint.
• If its variable is unbound, set the value of this variable to the value in the constraint and change state to bound.
• If its variable is bound and its value is an unbound variable, swap the variable and the value of the constraint, and do as above.
• Otherwise, unlink the constraint and unify the value of the variable with the value of the constraint. This may fail, interrupting entering.

Then, push an and-continuation corresponding to the continuation of the andnode. Finally, wake.

Note that when installing simply, only case two is applicable when entering an and-node, since failure or entailment cannot occur due to stability.

#### 6.4.11 Choice Splitting

To attempt choice splitting:

• Select a candidate. If none is found, back up.
• If the candidate's grandparent is the current and-node, then, if it has only one sibling, which is solved, push a promote task referring to the sibling, otherwise, push a choice splitting task referring to the current and-node.
• Copy w.r.t. the candidate.
• Install (simply) the copy of the candidate and proceed.

To select a candidate, look for solved and-nodes in the current and-node, the continuations of which refer to guard_nondet instructions. Select one according to the chosen scheme, for example, the left-most candidate. (See Section 6.7.8 for a some remarks on other possibilities.)

To copy w.r.t. a candidate, consider the and-node which is the grandparent of the candidate. Make an isomorphic copy of this node and its attributes, recursively, and insert it to the left of the grandparent, with the following three exceptions: (1) Do not copy the siblings of the candidate—make the copy of the candidate the only alternative. (2) Do not copy variables that are external to the copied node. (3) Do not copy suspensions that refer to dead and-nodes or to the candidate. For each external variable encountered, refer to it also from the copy, and for each of its suspensions that refers to a copied node, add a new suspension that refers to the copy, deleting the original suspension if it refers to the candidate. Finally, unlink the candidate and mark it as dead.

Note that it is permitted to dereference all variable chains during copying, since all local bindings are deinstalled, and the copied subtree is stable. (The single exception is for variables which are used as accumulators in aggregates.)

#### 6.4.12 Collecting

To collect the current and-node, change its state to dead, unlink it from alternatives, and set the forward attribute to refer to the parent of the current choicenode. Store a reference to the next instruction in the current and-continuation, and set the and-attribute in the current context to the top of the stack. If there is a left alternative, install it, otherwise install the right alternative. Proceed.

Note that by putting the continuation in the context of the parent and-node, it will be executed after having promoted the unit or after backing up. Note also that if there is a left alternative, it may be a collect alternative, whereas the right alternative might be the final unit alternative. (See Section 6.6.6 for how these relate to the aggregate statements.) Note, finally, that precisely this moving of an and-task "across" choice-tasks is what makes it necessary to have two separate task stacks instead of just one. (See Section 6.9.2 for a discussion of alternatives.)
