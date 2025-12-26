# Thesis Part 12

## AN ABSTRACT MACHINE (continued)

A constraint v = t, where v is single, is rewritten to true. A constraint u = v, where v is a temporary variable and u is not, is rewritten to v = u. A constraint v = t, where neither v nor t are temporary, is rewritten to (u = v, u = t), where u is a new local variable.

A definition of the form

```
p(v₁, …, vₙ) := V : A
```

where i-1 is the parameter number of vᵢ, may be compiled to code of the form

```
allocate N
〈save parameters〉
〈code for body〉
deallocate
proceed
```

where N is the number of permanent variables. To each permanent variable is assigned a (unique) y-register yᵢ, for i in 0, …, N–1. Rules for assigning x-registers to temporary variables are given in Section 6.6.8, as constraints on the code generated. For a permanent variable, Rv stands for the y-register which it is already assigned. For a temporary variable, Rv serves as a place-holder, which will be substituted by an x-register.

To save the parameters v₁, …, vₙ, produce the code

```
get_variable Rv₁ xᵢ₁
…
get_variable Rvₙ xᵢₙ
```

where iⱼ is the parameter number of vⱼ.

### 6.6.3 Atoms

A constraint atom

```
u = v
```

is compiled to

```
put_variable Rv Ru
```

if both are first, to

```
put_value Rv Ru
```

if first occurrence of u, to

```
get_variable Rv Ru
```

if first occurrence of v, and to

```
get_value Rv Ru
```

otherwise.

A constraint atom

```
v = S
```

where S is a symbol, is compiled to

```
put_symbol S Rv
```

if v is first, and to

```
get_symbol S Rv
```

otherwise.

A constraint atom

```
v = f(t₁, …, tₙ)
```

is compiled to

```
X_constructor F Rv
Y₁
…
Yₙ
```

where F is f/n, X is put if v is first, otherwise get, and Yᵢ is

```
unify_void
```

if tᵢ is a variable and single,

```
unify_variable Ru
```

if tᵢ is a variable u and first,

```
unify_value Ru
```

if tᵢ is a variable u and not first, and

```
unify_symbol S
```

if tᵢ is a symbol S.

The constraint true produces no code.

A program atom

```
p(v₁, …, vₙ)
```

is compiled to

```
Y₁
…
Yₙ
call p_N N
```

where p_N is the label of the code for the definition of p/N, and where Yᵢ is

```
put_void xᵢ₋₁
```

if vᵢ is single,

```
put_variable Rvᵢ xᵢ₋₁
```

if vᵢ is first, and

```
put_value Rvᵢ xᵢ₋₁
```

otherwise.

### 6.6.4 Choice

A definition of the form

```
p(v₁, …, vₙ) := (〈clause₁〉 ; … ; 〈clauseₙ〉)
```

where i-1 is the parameter number of vᵢ, may be compiled to code of the form

```
p_N: switch_on_tree Lv Ls Lc
Lv: 〈try-retry-trust N C₁, …, Cₙ〉
Ls: 〈try-retry-trust N Cᵢ₁, …, Cᵢₖ〉
Lc: 〈try-retry-trust N Cⱼ₁, …, Cⱼₘ〉
C₁: 〈code for clause₁〉
…
Cₙ: 〈code for clauseₙ〉
```

where C₁, …, Cₙ, Lv, Ls, and Lc are local names that only pertain to the code for the choice statement at hand. Clauses other than Cᵢ₁ to Cᵢₖ are known to fail (in the guard) if the contents of x₀ is a symbol. Clauses other than Cⱼ₁ to Cⱼₘ are known to fail (in the guard) if the contents of x₀ is a tree constructor.

A zero length try-retry-trust chain is given as

```
fail
```

a chain Cᵢ, of length one, as

```
try_only Cᵢ
```

and longer chains, Cᵢ₁, …, Cᵢₖ as

```
try Cᵢ₁ N
retry Cᵢ₂
…
trust Cᵢₖ
```

The try-retry-trust instructions build, incrementally, the choice-box and guarded goals for a choice statement. The switch_on_tree instruction makes a first choice, eliminating clauses that are known to be incompatible. Better clause selection is possible, e.g., by decision graphs [Brand 1994]. The switch_on_tree instruction only illustrates the concept.

### 6.6.5 Clauses

A clause is very similar to a composition statement. The chunks of a clause are the chunks of the guard and of the body. The parameters are the parameters of the head of the definition in which they occur, which also gives the parameter numbers. The local variables are given in the hiding over the clause, which is otherwise ignored. The notions of first, single, permanent, and temporary are otherwise as for composition.

A clause may be compiled as

```
allocate N
〈save parameters〉
〈code for guard〉
guard_%
〈code for body〉
deallocate
proceed
```

with the same conditions as for composition.

### 6.6.6 Aggregates

The treatment of aggregates is somewhat unclean¹, but adequate since it lends itself to simple implementation ("worse is better" [Gabriel 1994]).

Observe that the variable created and stored in xₘ is bound to the contents of xᵢ. The collect instructions will replace the value of this variable repeatedly. The variable itself will not be copied by choice splitting in the aggregate, since it is external.

A definition of the form

```
p(v₁, …, vₘ) := aggregate(u, A, w)
```

where i-1 is the parameter number of vᵢ, and w is vⱼ, may be compiled to code of the form

```
p_N: put_variable xₘ₊₁ xₘ
     get_value xₘ₊₁ xⱼ₋₁
     try L₁ M+1
     trust L₂

L₁:  allocate N₁+1
     get_variable yₙ₁ xₘ
     〈save parameters〉
     〈code for A〉
     guard_collect yₙ₁ Rv Rv′
     〈code for collect(u, v, v′)〉
     deallocate
     proceed

L₂:  allocate N₂+1
     get_variable yₙ₂ xₘ
     guard_unit yₙ₂ Rv
     〈code for unit(v)〉
     deallocate
     proceed
```

The code generated corresponds to that for a choice with two special clauses. In the first are generated the solutions for u in A, which are collected by the guard, and the second ends with the unit when there are no more solutions.

The chunks of the collect clause are the chunks in A and in collect(u, v, v′). The chunks of the unit clause are the chunks in unit(v). Variables are classified and allocated accordingly, with the exception that v and v' have their first occurrences in the guard instructions and that parameters that occur in the unit are not temporary. N₁ and N₂ are the respective numbers of permanent variables.

### 6.6.7 Initial Statements

For simplicity, we assume that the initial statement is a program atom

```
initial(v₁, …, vₙ)
```

This is no restriction, since it may be defined as anything.

```
     try_only L
L:   allocate N
     put_variable y₀ x₀
     …
     put_variable yₙ₋₁ xₙ₋₁
     call initial_N N
     guard_top
```

The guard_top instruction is given full freedom to report solutions for the variables in this statements, which are stored in y-registers, or other actions that would seem useful. For simplicity, we have given it the semantics of the execution model, where execution continues with other branches.

### 6.6.8 Register Allocation

The *lifetime* of a temporary variable is the sequence of instructions between, not including, the first and last instructions using Rv.

To *assign x-registers*, for each temporary variable v, select a register not used in its lifetime, and substitute it for Rv.

The objectives for this process are to minimise the number of x-registers used and to make possible the deletion of instructions as described in the next section

### 6.6.9 Editing

Having generated code according to the principles in preceding sections, some editing should be performed to improve execution efficiency.

The following instructions have no effect and can be deleted

```
get_variable xᵢ xᵢ
get_value xᵢ xᵢ
put_value xᵢ xᵢ
```

The instruction sequence

```
call L N
deallocate
proceed
```

should be replaced by

```
deallocate
execute L N
```

to ensure that tail recursive programs do not grow the and-stack.

The allocate instruction can be moved to just before the first instruction using a y-register, the first call, or the guard instruction, whichever comes first. The deallocate instruction can be moved to just after the last instruction using a y-register, the last call instruction, or the guard instruction, whichever comes last.

### 6.6.10 Two Examples

The familiar definition of append

```
append([], Y, Y).
append([H|X], Y, [H|Z]) :-
    append(X, Y, Z).
```

which is written as follows without syntactic sugar

```
append(X, Y, Z) :=
  ( X = [],
    Y = Z
  ? true
  ; H, X₁, Z₁ :
    X = [H|X₁],
    Z = [H|Z₁]
  ? append(X₁, Y, Z₁) ).
```

can be translated to the following code using the principles above. Observe that ./2 is regarded as the functor for list constructors.

```
append_3:
    switch_on_tree Lv Ls Lc
Lv: try C₁ 3
    trust C₂
Ls: try_only C₁
Lc: try_only C₂

C₁: allocate 0
    get_symbol [] x₀
    get_value x₁ x₂
guard_nondeterminate
    deallocate
    proceed

C₂: allocate 3
    get_variable y₁ x₁
    get_constructor ./2 x₀
    unify_variable x₁
    unify_variable y₀
    get_constructor ./2 x₂
    unify_value x₁
    unify_variable y₂
guard_nondeterminate
    put_value y₀ x₀
    put_value y₁ x₁
    put_value y₂ x₂
    deallocate
    execute append3
```

The guard instructions are given less indentation for readability.

In particular, code is generated for the second clause

```
H, X₁, Z₁ :
  X = [H|X₁],
  Z = [H|Z₁]
? append(X₁, Y, Z₁)
```

as follows. The variables X, Y, and Z, are parameters with numbers 0, 1, and 2, respectively. The variables X₁, Y, and Z₁ are classified as permanent and are assigned y-registers 0, 1, and 2, respectively. The variables X, H, and Z are temporary. The code before x-register allocation is

```
allocate 3
get_variable Rₓ x₀
get_variable y₁ x₁
get_variable Rz x₂
get_constructor ./2 Rₓ
unify_variable Rₕ
unify_variable y₀
get_constructor ./2 Rz
unify_value Rₕ
unify_variable y₂
guard_nondet
put_value y₀ x₀
put_value y₁ x₁
put_value y₂ x₂
call append_3 3
deallocate
proceed
```

The variables X, H, and Z can now be allocated x-registers 0, 1, and 2, respectively, following the principles of lifetimes. Finally, two of the get_variable instructions can be deleted, and the call-deallocate-proceed instructions can be replaced by corresponding deallocate-execute instructions.

A call to bagof, such as the one discussed in Section 6.8.1,

```
p(L, S) := bagof(X, tail(X, L), S)
```

can be translated to the following code using the principles above

```
p_2: put_variable x₃ x₂
     get_value x₃ x₁
     try L₁ 3
     trust L₂

L₁:  allocate 2
     get_variable y₁ x₂
     get_variable x₁ x₀
     put_variable y₀ x₀
     call tail_2 2
     guard_collect y₁ x₀ x₁
     get_constructor ./2 x₁
     unify_value y₀
     unify_value x₀
     deallocate
     proceed

L₂:  allocate 1
     get_variable y₀ x₂
     guard_unit y₀ x₀
     get_symbol [] x₀
     deallocate
     proceed
```

assuming that guard_collect is ordered. It is of course perfectly possible to provide both ordered and unordered versions of this instruction for use in different aggregates.

## 6.7 OPTIMISATIONS

This section lists a few optimisations that are available in the AGENTS system, and others that are believed to be obvious for future efficient implementations.

### 6.7.1 Flat Guards

Using only the simple machinery introduced so far, the execution speed of the AGENTS system is roughly a factor of four slower, for comparable programs without don't know nondeterminism, than a comparable Prolog implementation (such as SICStus Prolog with emulated code [Carlsson et al. 1993]), and much more memory is needed for execution. This is almost entirely due to the unnecessary creation of choice-nodes, and-nodes, and and-continuations for every choice statement.

However, a few simple instructions and notions of suspending and waking calls almost completely bridge the gap for a wide range of programs. The idea is to short-cut to promotion, suspension, or failure, for guards that only make simple tests on the first argument. This is a special case of the *flat guards*, that only contain constraints (with composition and hiding).

A *suspended call* has the attributes

• *parent*: a reference to an and-node
• *continuation pointer*: a reference to a sequence of instructions
• *a-registers*: a vector of trees
• *goals*: a pair of references to goals

Admit references to suspended calls in place of and-nodes in suspensions, in the wake stack, and in wake tasks.

To *suspend L with N arguments on a variable X*, create a suspended call with L as continuation pointer and N a-registers with contents from corresponding x-registers. Insert it at the insertion point, and add a suspension on the variable X referring to the suspended call.

To *proceed*, if the task is wake task referring to a suspended call, pop it. Install its parent and-node. Restore the program counter and x-registers from the suspended call, set the insertion pointer to its right sibling, unlink it, and decode instructions.

Add the following three instructions.

**switch_on_constructor Lf F₁-L₁ … Fₙ-Lₙ**
**switch_on_symbol Lf S₁-L₁ … Sₙ-Lₙ**

If x₀, dereferenced, is a constructor with functor Fᵢ (is a symbol Sᵢ), go to Lᵢ. Otherwise, go to Lf.

**suspend_call L N**

Suspend L with N arguments on the variable in x₀. Proceed.

A definition such as

```
q(X) :=
  ( X = a → true
  ; Y: X = f(Y) → q(Y) )
```

which had to be encoded as

```
q_1: switch_on_tree Lv Ls Lc
Lv:  try C₁ 1
     trust C₂
Ls:  try_only C₁
Lc:  try_only C₂

C₁:  allocate 0
     get_symbol a x₀
     guard_nondet
     deallocate
     proceed

C₁:  allocate 1
     get_constructor f/1 x₀
     unify_variable y₀
     guard_nondet
     put_value y₀ x₀
     deallocate
     execute q_1 1
```

can now be encoded as

```
q_1: switch_on_tree Lv Ls Lc
Lv:  suspend_call q_1 1
Ls:  switch_on_symbol Lf a-C₁
Lc:  switch_on_constructor Lf f/1-C₂
Lf:  fail
C₁:  proceed
C₂:  get_constructor f/1 x₀
     unify_variable x₀
     execute q_1 1
```

| Example | AGENTS without opt. | AGENTS with opt. | SICStus emulated |
|---------|---------------------|------------------|------------------|
| nreverse(300) | 610 (4.5) | 215 (1.6) | 137 (1.0) |
| nreverse(1000) | 7499 (2.9) | 2433 (0.9) | 2612 (1.0) |
| sort(medi) | 394 (3.8) | 250 (2.4) | 105 (1.0) |
| sort(maxi) | 8613 (4.1) | 4031 (1.9) | 2077 (1.0) |

**Figure 6.6. Performance of AGENTS w/wo optimisation vs. SICStus Prolog**

The behaviour of the guards is entirely captured by the combination of switch instructions and the suspend_call instruction, and neither choice-nodes, and-nodes, nor and-continuations have to be created.

The impact on the performance of AGENTS for simple benchmarks where this coding of flat guards is applicable is shown in Figure 6.6. The comparison with SICStus only serves to indicate that the execution speed of AGENTS ends up in an acceptable order of magnitude.

The systems compared are AGENTS 0.9, with and without the above flat guard optimisation, and SICStus Prolog 2.1 #8 (emulated code) on a DECstation 5000/240. The times are in milliseconds and include the time for garbage collec-

---

¹ Observe the appropriate section number.
