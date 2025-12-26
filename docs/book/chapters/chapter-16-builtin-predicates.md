# Chapter 16: Built-in Predicates

## Introduction

The AGENTS emulator provides an extensive library of **built-in predicates** — primitive operations implemented in C that extend the basic capabilities of the AKL language. While the PAM instruction set handles core language constructs like unification, goal invocation, and backtracking, built-in predicates provide essential functionality for:

- **Term inspection and testing** (checking types, accessing structure)
- **Arithmetic evaluation** (mathematical operations and comparisons)
- **Input/Output** (reading and writing terms, file operations)
- **Meta-programming** (reflection, predicate manipulation)
- **System interaction** (statistics, debugging, foreign function calls)
- **Specialized data structures** (arrays, hash tables, rational numbers)

The emulator includes approximately **134 built-in predicates** distributed across 22 implementation files, each focused on a specific domain. This chapter examines the architecture of the built-in system, the implementation patterns used consistently across all built-ins, and representative examples from major categories.

## Architectural Overview

Built-in predicates bridge the gap between the high-level AKL language and low-level C implementation. They follow a uniform calling convention that integrates seamlessly with:

- The **suspension mechanism** for constraint programming
- The **unification engine** for binding results
- The **error handling system** for reporting domain/type errors
- The **garbage collector** for memory management
- The **execution state** for accessing registers and environments

The built-in system consists of three main components:

1. **Implementation modules** (`builtin.c`, `is.c`, `inout.c`, etc.) containing C functions
2. **Initialization system** that registers predicates with the emulator
3. **Invocation mechanism** that calls C functions from bytecode

## Implementation Pattern

All built-in predicates follow a consistent implementation pattern that handles argument access, type checking, suspension, and result unification. Understanding this pattern is crucial for both implementing new built-ins and analyzing existing ones.

### Function Signature

Every built-in predicate has the signature:

```c
bool akl_predicate_name(Arg)
    Argdecl;
```

The `Argdecl` macro expands to provide access to:
- `exstate *exs` — Current execution state (registers, trail, heap, etc.)
- Argument access macros `A(n)` that retrieve the nth argument
- Access to the current and-box via `exs->andb`

### Argument Dereferencing

Built-ins must **dereference** arguments before use, following variable chains to their ultimate values:

```c
bool akl_atom(Arg)
    Argdecl;
{
  register Term X0;
  Deref(X0, A(0));          // Follow variable chain
  IfVarSuspend(X0);         // Suspend if still unbound
  return IsATM(X0);         // Check if atom
}
```

The `Deref` macro follows reference chains until reaching:
- An **unbound variable** (UVA)
- A **suspended variable** (SVA)
- A **constrained variable** (CVA)
- A **concrete value** (atom, number, structure, list)

### Suspension Integration

Type-checking predicates use `IfVarSuspend` to suspend execution when encountering unbound variables. This macro:

1. Checks if the term is an unbound variable
2. If so, adds the current agent to the variable's suspension list
3. Returns `SUSPEND` to the emulator
4. The agent will be reactivated when the variable becomes bound

```c
bool akl_integer(Arg)
    Argdecl;
{
  register Term X0;
  Deref(X0, A(0));
  IfVarSuspend(X0);          // Wait if variable
  return IsINT(X0);          // Otherwise check type
}
```

This allows queries like `integer(X)` to **wait** until `X` becomes instantiated, a fundamental feature of constraint programming in AKL.

### Return Values

Built-in predicates return one of three values:

- **`TRUE`** — Predicate succeeded
- **`FALSE`** — Predicate failed (triggers backtracking)
- **`SUSPEND`** — Execution suspended (agent waits for variable binding)

The return value controls the emulator's next action after the built-in completes.

### Result Unification

Built-ins that compute results use the unification engine to bind output arguments:

```c
bool akl_functor_size(Arg)
    Argdecl;
{
  register Term X0, X1;
  Term result;

  Deref(X0, A(0));
  if (!IsFUNC(X0)) {
    IfVarSuspend(X0);
    DOMAIN_ERROR("functor_size", 2, 1, "functor")
  }

  Deref(X1, A(1));
  MakeIntegerTerm(result, FnkArity(Fnk(X0)));
  return unify(X1, result, exs->andb, exs);
}
```

The `unify` function handles the complete unification process, including:
- Trailing for backtracking
- Occurs check (if enabled)
- Wake queue management for suspended agents

## Registration and Initialization

Built-in predicates are registered with the emulator during startup through a systematic initialization process.

### The `define()` Function

The `define()` function registers a C function as a callable predicate:

```c
void define(pname, procedure, arity)
    char *pname;              // Predicate name
    bool (*procedure)();      // Function pointer
    int arity;                // Number of arguments
{
  define_c_predicate(store_atom(pname), arity, procedure);
}
```

This creates a **predicate descriptor** that associates the predicate name with:
- The C function implementing it
- Its arity
- Metadata for the emulator

### Module Initialization Functions

Each implementation file provides an `initialize_*` function that registers its predicates. For example, from `builtin.c:1626-1676`:

```c
void initialize_builtin() {
  term_list = TagAtm(store_atom("."));

  // Variable testing
  define("$var_descriptor", akl_var_descriptor, 2);
  define("$var", akl_var, 1);
  define("$eq", akl_eq, 2);

  // Type checking
  define("data", akl_data, 1);
  define("atom", akl_atom, 1);
  define("integer", akl_integer, 1);
  define("float", akl_float, 1);
  define("atomic", akl_atomic, 1);
  define("compound", akl_compound, 1);
  define("tree", akl_tree, 1);

  // String conversions
  define("atom_char", akl_atom_char, 3);
  define("atom_to_chars", akl_atom_to_chars, 2);
  define("integer_to_chars", akl_integer_to_chars, 2);
  define("float_to_chars", akl_float_to_chars, 2);
  define("chars_to_atom", akl_chars_to_atom, 2);
  define("chars_to_integer", akl_chars_to_integer, 2);
  define("chars_to_float", akl_chars_to_float, 2);

  // Functor operations
  define("tree_to_functor", akl_tree_to_functor, 2);
  define("functor_to_tree", akl_functor_to_tree, 2);
  define("functor", akl_functor, 1);
  define("functor_size", akl_functor_size, 2);
  define("functor_arity", akl_functor_arity, 2);
  define("functor_name", akl_functor_name, 2);
  define("functor_features", akl_functor_features, 2);
  define("functor_feature", akl_functor_feature, 3);
  define("name_arity_to_functor", akl_name_arity_to_functor, 3);

  // Structure access
  define("arg", akl_arg, 3);
  define("sys_setarg", akl_sys_setarg, 3);

  // List operations
  define("$islist", akl_islist, 1);
  define("$concat", akl_concat, 3);
  define("$memberchk", akl_memberchk, 2);

  // List/length conversions
  define("list_to_length", akl_list_to_length, 2);
  define("length_to_list", akl_length_to_list, 2);

  // Constraint predicates
  define("dif", akl_dif, 2);
}
```

### System Startup Sequence

The `initialise()` function in `initial.c:215-233` orchestrates the complete startup sequence:

```c
void initialise()
{
  check_architecture();          // Verify tag scheme compatibility
  init_alloc();                  // Initialize memory allocators
  init_code();                   // Initialize code area
  init_atomtable();              // Initialize atom table
  init_predtable();              // Initialize predicate table
  init_functortable();           // Initialize functor table
  init_constants();              // Create standard atoms

  init_halt();                   // Initialize halt continuation
  init_proceed();                // Initialize proceed continuation
  init_fail_dec();               // Initialize fail/dec mechanism

  init_intrinsics();             // Register all built-in predicates

  init_each_time();              // Per-execution initialization
}
```

The `init_intrinsics()` function calls initialization for all modules (`initial.c:109-144`):

```c
static void init_intrinsics() {
  initialize_time();             // Time and timing predicates
  initialize_is();               // Arithmetic evaluation (is/2)
  initialize_compare();          // Term comparison
  initialize_port();             // Port communication
  initialize_builtin();          // General built-ins
  initialize_bignum();           // Big integer arithmetic
  #ifdef RATNUM
  initialize_rational();         // Rational number arithmetic
  #endif
  initialize_array();            // Array operations
  initialize_hash();             // Hash table operations
  initialize_storage();          // Memory management predicates
  initialize_foreign();          // Foreign function interface
  initialize_load();             // Dynamic code loading
  initialize_inout();            // I/O operations
  initialize_statistics();       // Runtime statistics
  initialize_candidate();        // Candidate selection control
  initialize_reflection();       // Reflection and meta-predicates
  initialize_display();          // Term display
  initialize_debug();            // Debugging predicates
  initialize_trace();            // Execution tracing
  initialize_predicate();        // Predicate manipulation
  initialize_aggregate();        // Aggregate operations
  initialize_finite();           // Finite domain constraints
  initialize_error();            // Error handling
  initialize_abstraction();      // Abstraction support

  define("agents_features", agents_features, 1);

  install_foreign_predicates();  // Install user-defined foreign predicates
}
```

This modular initialization allows new categories of built-ins to be added by:
1. Creating a new implementation file
2. Writing an `initialize_*` function
3. Adding a call to `init_intrinsics()`

## Categories of Built-in Predicates

The 134 built-in predicates span 22 implementation files, organized by functional domain. We examine representative examples from each major category.

### Term Testing Predicates

These predicates check the type or properties of terms. They suspend on unbound variables and return boolean results.

**Type Checking:**

```c
// builtin.c:126-131
bool akl_atom(Arg)
    Argdecl;
{
  register Term X0;
  Deref(X0, A(0));
  IfVarSuspend(X0);
  return IsATM(X0);
}

// Similar predicates:
// akl_integer/1, akl_float/1, akl_number/1 (integer or float)
// akl_atomic/1 (atom, integer, or float)
// akl_compound/1 (structure or list)
// akl_tree/1 (any valid term)
```

**Variable Testing:**

```c
// builtin.c:96-102
bool akl_var(Arg)
    Argdecl;
{
  register Term X0;
  Deref(X0, A(0));
  if(IsVar(X0)) return TRUE;    // Note: Does NOT suspend!
  else return FALSE;
}

// akl_data/1 - opposite of var/1
```

The distinction between `akl_var` and `akl_atom` is crucial:
- `var(X)` succeeds immediately if `X` is unbound, fails otherwise
- `atom(X)` **suspends** if `X` is unbound, waits for binding

This allows AKL programs to test for variables explicitly:

```prolog
check_term(X) :=
    ( var(X) ->
        write('Still a variable')
    ; atom(X) ->
        write('Now an atom')
    ; write('Something else')
    ).
```

### Term Manipulation Predicates

These predicates access and construct term components.

**String/Atom Conversion:**

```c
// builtin.c:295-310
bool akl_atom_to_chars(Arg)
    Argdecl;
{
  Term X0, X1, lst;

  Deref(X0, A(0));
  if (!IsATM(X0)) {
    IfVarSuspend(X0);
    DOMAIN_ERROR("atom_to_chars", 2, 1, "atom")
  }

  lst = make_string_list(AtmPname(Atm(X0)), exs);
  Deref(X1, A(1));
  return unify(X1, lst, exs->andb, exs);
}

// Similar: integer_to_chars/2, float_to_chars/2
//          chars_to_atom/2, chars_to_integer/2, chars_to_float/2
```

**Functor Operations:**

Functors represent the "skeleton" of structures — name and arity without arguments.

```c
// builtin.c:498-525
bool akl_tree_to_functor(Arg)
    Argdecl;
{
  register Term X0, X1, func;

  Deref(X0, A(0));
  IfVarSuspend(X0);

  Deref(X1, A(1));

  if (IsLST(X0)) {
    // Lists have functor './2'
    Functor f = store_functor(Atm(term_list), 2);
    return unify(X1, TagGen(f), exs->andb, exs);
  } else if (IsSTR(X0)) {
    // Extract functor from structure
    func = TagGen(StrFunctor(Str(X0)));
    return unify(X1, func, exs->andb, exs);
  } else if (IsATM(X0) || IsINT(X0) || IsFLT(X0)) {
    // Atomic terms are their own functors with arity 0
    functor0 *f;
    NewHeapFunctor(f);
    FnkName(f) = X0;
    return unify(X1, TagGen(f), exs->andb, exs);
  } else {
    DOMAIN_ERROR("tree_to_functor", 2, 1, "tree")
  }
}
```

**Functor Introspection:**

```c
// builtin.c:692-707
bool akl_functor_size(Arg)
    Argdecl;
{
  register Term X0, X1;
  Term trm;

  Deref(X0, A(0));
  if (!IsFUNC(X0)) {
    IfVarSuspend(X0);
    DOMAIN_ERROR("functor_size", 2, 1, "functor")
  }

  Deref(X1, A(1));
  MakeIntegerTerm(trm, FnkArity(Fnk(X0)));
  return unify(X1, trm, exs->andb, exs);
}

// Similar: functor_name/2, functor_arity/2, functor_features/2
```

**Structure Access:**

```c
// builtin.c (simplified from actual implementation)
bool akl_arg(Arg)
    Argdecl;
{
  Term X0, X1, X2, result;
  int index;

  // Arg 0: index
  Deref(X0, A(0));
  if (!IsINT(X0)) {
    IfVarSuspend(X0);
    DOMAIN_ERROR("arg", 3, 1, "integer")
  }
  index = GetInteger(X0);

  // Arg 1: structure or list
  Deref(X1, A(1));
  IfVarSuspend(X1);

  if (IsSTR(X1)) {
    Structure s = Str(X1);
    if (index < 1 || index > FnkArity(StrFunctor(s))) {
      return FALSE;  // Index out of bounds
    }
    GetArg(result, StrArgRef(s, index-1));
  } else if (IsLST(X1)) {
    // Lists treated as './2' structures
    if (index == 1) {
      GetArg(result, LstCarRef(Lst(X1)));
    } else if (index == 2) {
      GetArg(result, LstCdrRef(Lst(X1)));
    } else {
      return FALSE;
    }
  } else {
    DOMAIN_ERROR("arg", 3, 2, "compound term")
  }

  Deref(X2, A(2));
  return unify(X2, result, exs->andb, exs);
}
```

Example usage:

```prolog
?- tree_to_functor(foo(a,b), F).
F = foo/2

?- functor_size(F, N), functor_name(F, Name).
N = 2, Name = foo

?- functor_to_tree(foo/2, T).
T = foo(_,_)  % Fresh variables

?- arg(1, foo(a,b), X).
X = a

?- arg(2, [head|tail], X).
X = tail  % Lists are './2'
```

### Arithmetic Evaluation

The `is.c` module implements arithmetic evaluation for the `is/2` predicate and arithmetic constraints.

**Arithmetic Functors:**

```c
// is.c:17-56
Functor functor_add;     // +/2
Functor functor_sub;     // -/2
Functor functor_mul;     // */2
Functor functor_div;     // //2
Functor functor_idiv;    // div/2
Functor functor_mod;     // mod/2
Functor functor_min;     // min/2
Functor functor_max;     // max/2
Functor functor_and;     // /\/2
Functor functor_or;      // \//2
Functor functor_xor;     // xor/2
Functor functor_shl;     // <</2
Functor functor_shr;     // >>/2
Functor functor_neg;     // -/1
Functor functor_plus;    // +/1
Functor functor_integer; // integer/1
Functor functor_com;     // \\/1
Functor functor_float;   // float/1

// Transcendental functions
Functor functor_sin, functor_cos, functor_tan;
Functor functor_asin, functor_acos, functor_atan, functor_atan2;
Functor functor_sinh, functor_cosh, functor_tanh;
Functor functor_exp, functor_log, functor_log10, functor_pow;
Functor functor_sqrt, functor_fabs, functor_ceil, functor_floor;
```

**Evaluation Macros:**

The implementation uses macros to generate arithmetic functions that handle:
- **Small integers** (immediate values)
- **Floating-point numbers**
- **Big integers** (via GMP library)

```c
// is.c:59-82
#define BinaryEvalCast(Name, Expr, Bignumfunc) \
void Name(tmp1, tmp2, val) \
     Term tmp1, tmp2; \
     Term *val; \
{ \
  if (IsNUM(tmp1) && IsNUM(tmp2)) { \
    sword a, b; \
    a = GetSmall(tmp1); \
    b = GetSmall(tmp2); \
    MakeIntegerTerm(*val, (Expr)); \
    return; \
  } \
 \
  if (IsFLT(tmp1) || IsFLT(tmp2)) { \
    double a, b; \
    a = get_float(tmp1); \
    b = get_float(tmp2); \
    MakeFloatTerm(*val, (Expr)); \
    return; \
  } \
 \
  *val = Bignumfunc(tmp1, tmp2); \
  return; \
}
```

This pattern allows operators like addition to work transparently across number types:

```prolog
?- X is 5 + 3.
X = 8

?- X is 5.0 + 3.
X = 8.0

?- X is 99999999999999999999 + 1.
X = 100000000000000000000  % Automatic bignum promotion
```

**Arithmetic Comparison:**

```c
// compare.c (simplified)
bool akl_less_than(Arg)
    Argdecl;
{
  Term X0, X1;

  Deref(X0, A(0));
  Deref(X1, A(1));

  // Suspend if either argument is variable
  IfVarSuspend(X0);
  IfVarSuspend(X1);

  // Compare numeric values
  return compare_numbers(X0, X1) < 0;
}

// Similar: =:=/2, =\=/2, </2, >/2, =</2, >=/2
```

### Input/Output Predicates

The `inout.c` module provides I/O operations built on **stream objects** — constrained variables with associated file handles.

**Stream Structure:**

```c
// inout.c:29-34
typedef struct stream {
  struct gvamethod *method;     // CVA method table
  envid           *env;         // Owning environment
  FILE            *file;        // C file handle
  struct ptrdict   dict;        // Cycle detection for term output
} stream;
```

Streams are **CVA (Constrained Variable Abstractions)** with methods for:
- `newstream` — Create new stream
- `copystream` — Copy during and-box promotion
- `gcstream` — Garbage collect stream
- `sendstream` — Port-style message passing
- `unifystream` — Custom unification behavior
- `printstream` — Display stream
- `deallocatestream` — Clean up when environment dies
- `killstream` — Force-close stream

**Built-in I/O Predicates:**

```c
// From inout.c initialization (not shown in snippets, but inferred)

initialize_inout() {
  define("see", akl_see, 1);           // Open file for reading
  define("seen", akl_seen, 0);         // Close current input
  define("seeing", akl_seeing, 1);     // Query current input stream
  define("tell", akl_tell, 1);         // Open file for writing
  define("told", akl_told, 0);         // Close current output
  define("telling", akl_telling, 1);   // Query current output stream
  define("get", akl_get, 1);           // Read character
  define("get0", akl_get0, 1);         // Read character (including whitespace)
  define("put", akl_put, 1);           // Write character
  define("nl", akl_nl, 0);             // Write newline
  define("read", akl_read, 1);         // Read term
  define("write", akl_write, 1);       // Write term
  define("writeq", akl_writeq, 1);     // Write quoted term
  define("display", akl_display, 1);   // Write term without operators
  // ... and more
}
```

Example usage:

```prolog
% Open file for writing
?- tell('output.txt').

% Write terms
?- write('Hello, '), write(world), write('!'), nl.

% Close file
?- told.

% Read from file
?- see('input.txt'), read(Term), seen.
Term = foo(bar, baz)
```

### Statistics and Runtime Information

The `statistics.c` module provides predicates for querying runtime metrics.

**Runtime Measurement:**

```c
// statistics.c:33-56
bool akl_statistics_runtime(Arg)
    Argdecl;
{
  int tmp;
  register Term X0, X1;
  Term tex, tsex;

  Deref(X0, A(0));
  Deref(X1, A(1));

  tmp = systime();        // Get current time (milliseconds)

  MakeIntegerTerm(tex, (tmp - exectime));    // Total time
  MakeIntegerTerm(tsex, (tmp - sexectime));  // Since last call

  if (unify(X0, tex, exs->andb, exs) &&
      unify(X1, tsex, exs->andb, exs)) {
    sexectime = tmp;
    return TRUE;
  } else {
    sexectime = tmp;
    return FALSE;
  }
}
```

**Garbage Collection Statistics:**

```c
// statistics.c:84-103 (simplified)
bool akl_statistics_gctime(Arg)
    Argdecl;
{
#ifdef METERING
  register Term X0, X1;
  Term gct, gcts;

  Deref(X0, A(0));
  Deref(X1, A(1));

  MakeIntegerTerm(gct, gctime);                // Total GC time
  MakeIntegerTerm(gcts, (gctime - sgctime));   // Since last call

  if (unify(X0, gct, exs->andb, exs) &&
      unify(X1, gcts, exs->andb, exs)) {
    sgctime = gctime;
    return TRUE;
  } else {
    return FALSE;
  }
#else
  return FALSE;  // Not compiled with metering
#endif
}
```

Example usage:

```prolog
?- statistics(runtime, [Total, Since]).
Total = 5234,    % 5.234 seconds total
Since = 127      % 0.127 seconds since last call

?- statistics(gctime, [Total, Since]).
Total = 823,
Since = 45
```

### Reflection and Meta-predicates

The `reflection.c` module provides introspective capabilities for examining the execution state.

**Reflection Objects:**

```c
// reflection.c:37-43
typedef struct reflection {
  gvamethod *method;        // CVA method table
  envid     *env;          // Owning environment
  exstate   *exstate;      // Captured execution state
  Term       stream;       // Output stream for solutions
  constraint *constr;      // Associated constraints
} reflection;
```

Reflection objects allow AKL programs to:
- Capture the current execution state
- Query variable bindings
- Enumerate solutions to goals
- Implement custom search strategies

Built-ins for reflection (from `reflection.c`, specific implementations not shown in snippets):

```prolog
% bagof/3, setof/3 - Collect solutions to a goal
% findall/3 - Find all solutions
% copy_term/2 - Create independent copy of term
% numbervars/3 - Replace variables with ground terms
% term_variables/2 - Extract list of variables
```

### Tracing and Debugging

The `trace.c` module implements execution tracing for debugging AKL programs.

**Trace State:**

```c
// trace.c:47-56
#ifdef TRACE
int identity;  // Unique ID for trace events
#endif

predicate *spypoints = (predicate *) &spypoints;  // Spy point list

int tracing = NOTRACE;       // Current trace mode
int debugging;               // Debugging enabled?
int failcatching;            // Catch failures?
int trace_full_cont;         // Trace full continuations?
int suspend_depth = SUSPEND_DEPTH_DEFAULT;
```

**Trace Levels:**

```c
// trace.c:62-67
#define Goal_Related (TRACE_CALL_GOAL | TRACE_EXIT_GOAL | \
                      TRACE_SUSPEND_GOAL | TRACE_REENTER_GOAL | \
                      TRACE_FAIL_GOAL)
#define Guard_Related (TRACE_CREATE_GUARD | TRACE_FAIL_GUARD | \
                       TRACE_SUSPEND_GUARD | TRACE_WAKE_GUARD)
#define Movement_Related (TRACE_UP_GUARD | TRACE_DOWN_GUARD)
#define Promotion_Related (TRACE_NOISY_PRUNING_GUARD | TRACE_PROMOTE_GUARD)
#define Nondeterminism_Related (TRACE_NON_DET_PRE | TRACE_NON_DET_POST)
```

Built-in tracing predicates (implementations in `trace.c`):

```prolog
% trace/0 - Enable full tracing
% notrace/0 - Disable tracing
% spy/1 - Set spy point on predicate
% nospy/1 - Remove spy point
% nospyall/0 - Clear all spy points
% debugging/0 - Show debugging status
% leash/1 - Configure trace ports
```

Example debugging session:

```prolog
?- spy(append/3).
% Spy point set on append/3

?- trace.
% Tracing enabled

?- append([1,2], [3,4], X).
 Call: append([1,2], [3,4], _1)
 Call: append([2], [3,4], _2)
 Call: append([], [3,4], _3)
 Exit: append([], [3,4], [3,4])
 Exit: append([2], [3,4], [2,3,4])
 Exit: append([1,2], [3,4], [1,2,3,4])
X = [1,2,3,4]
```

### Port Communication Built-ins

We covered ports extensively in Chapter 15. The `port.c` module provides:

```c
initialize_port() {
  define("new_port", akl_new_port, 1);       // Create new port
  define("send", akl_send, 2);               // Send message
  define("port_to_stream", akl_port_to_stream, 2);  // Access stream
  define("port_close", akl_port_close, 1);   // Close port
  define("is_port", akl_is_port, 1);         // Test if port
}
```

Ports integrate with the CVA mechanism to provide asynchronous, non-blocking communication between concurrent agents.

### Foreign Function Interface

The `foreign.c` module allows AKL programs to call external C functions.

**Foreign Predicate Registration:**

```c
// foreign.c (simplified)
bool akl_foreign(Arg)
    Argdecl;
{
  Term pred_name, arity, library;

  Deref(pred_name, A(0));
  Deref(arity, A(1));
  Deref(library, A(2));

  // Load shared library
  // Find function symbol
  // Register as callable predicate

  return install_foreign_predicate(pred_name, arity, library);
}
```

Example usage:

```prolog
% Load external C function
?- foreign(my_function, 2, 'libmylib.so').

% Call it like any predicate
?- my_function(input, Output).
Output = result
```

This allows extending AGENTS with domain-specific functionality without modifying the emulator.

### Specialized Data Structures

Several modules provide efficient implementations of common data structures.

**Arrays** (`array.c`):

```prolog
% array_create(+Size, -Array)
% array_get(+Array, +Index, -Value)
% array_set(+Array, +Index, +Value)
```

**Hash Tables** (`ghash.c`):

```prolog
% hash_create(+Size, -Hash)
% hash_insert(+Hash, +Key, +Value)
% hash_lookup(+Hash, +Key, -Value)
% hash_delete(+Hash, +Key)
```

**Rational Numbers** (`rational.c`, if compiled with `RATNUM`):

```prolog
% Rational arithmetic: 1/3 + 2/5 = 11/15
% rat_add(+R1, +R2, -R3)
% rat_mul(+R1, +R2, -R3)
% rat_compare(+R1, +R2, -Result)
```

## Error Handling

Built-in predicates use standardized error reporting macros.

### Error Types

```c
// error.h (not shown, but pattern evident from usage)

// Domain error: argument has wrong type
DOMAIN_ERROR(predicate, arity, argnum, expected_type)

// Representation error: value exceeds system limits
REPRESENTATION_ERROR(predicate, arity, argnum, description)

// Type error: more general type mismatch
TYPE_ERROR(predicate, arity, argnum, expected_type)

// Existence error: referenced object doesn't exist
EXISTENCE_ERROR(predicate, arity, object_type, object)
```

### Error Handling Example

```c
// builtin.c:246-291 (simplified)
bool akl_atom_char(Arg)
     Argdecl;
{
  register Term X0, X1, X2, tmp;
  int i;
  char *name;

  // Check first argument is integer
  Deref(X0, A(0));
  if (!IsINT(X0)) {
    IfVarSuspend(X0);
    DOMAIN_ERROR("atom_char", 3, 1, "integer")
  }

  if (!IsMachineInteger(X0))
    i = -1;
  else
    i = GetInteger(X0);

  // Check second argument is atom
  Deref(X1, A(1));
  if (!IsATM(X1)) {
    IfVarSuspend(X1);
    DOMAIN_ERROR("atom_char", 3, 2, "atom")
  }
  name = AtmPname(Atm(X1));

  // Check index in bounds
  if ((i >= strlen(name)) || (i < 0)) {
    char s[32];
    sprintf(s, "integer in 0..%d", strlen(name)-1);
    DOMAIN_ERROR("atom_char", 3, 1, s)
  }

  // Extract character
  tmp = MakeSmallNum(name[i]);
  Deref(X2, A(2));
  return unify(X2, tmp, exs->andb, exs);
}
```

Error macros generate **error terms** that the emulator can catch and report:

```prolog
?- atom_char(foo, 0, C).
ERROR: Domain error in atom_char/3, argument 1: expected integer

?- atom_char(0, hello, C).
ERROR: Domain error in atom_char/3, argument 2: expected atom

?- atom_char(10, hello, C).
ERROR: Domain error in atom_char/3, argument 1: integer in 0..4
```

## Integration with Constraint System

Built-in predicates integrate seamlessly with AKL's constraint programming features through the suspension mechanism.

### Suspending on Variables

Type-checking predicates suspend when encountering unbound variables:

```prolog
% Define constraint: X must be an atom when bound
atom_constraint(X) :- atom(X).

% X is currently unbound
?- atom_constraint(X).
% Agent SUSPENDS, waiting for X to be bound

% In parallel, bind X
?- X = hello.

% Agent WAKES UP, checks atom(hello), succeeds
```

The C implementation:

```c
bool akl_atom(Arg)
    Argdecl;
{
  register Term X0;
  Deref(X0, A(0));
  IfVarSuspend(X0);   // Suspend if variable, wake on binding
  return IsATM(X0);
}
```

The `IfVarSuspend` macro:
1. Checks if `X0` is an unbound variable (UVA)
2. If so, adds current agent to variable's suspension list
3. Returns `SUSPEND` to emulator
4. Emulator removes agent from task queue
5. When `X0` is bound, all suspended agents are reactivated

### Constraints on Built-in Operations

Some built-ins define **constraints** on arguments:

```c
// builtin.c:1400-1460 (simplified)
bool akl_dif(Arg)
    Argdecl;
{
  Term X0, X1;

  Deref(X0, A(0));
  Deref(X1, A(1));

  // If both ground, check inequality immediately
  if (!IsVar(X0) && !IsVar(X1)) {
    return !unifiable(X0, X1);
  }

  // Otherwise, install constraint that prevents unification
  return install_dif_constraint(X0, X1, exs);
}
```

Usage:

```prolog
?- dif(X, Y), X = a, Y = b.
X = a, Y = b  % Constraint satisfied

?- dif(X, Y), X = a, Y = a.
fail  % Constraint violated

?- dif(X, Y), X = foo(A), Y = foo(B).
% Constraint propagates: dif(A, B) must hold
```

## Performance Considerations

Built-in predicates are performance-critical, as they're invoked frequently during execution.

### Invocation Overhead

Calling a built-in predicate involves:

1. **Dispatch overhead** — Looking up predicate descriptor
2. **Argument marshalling** — Accessing arguments via `A(n)` macro
3. **Dereferencing** — Following variable chains
4. **Type checking** — Testing term types
5. **Result construction** — Building return values
6. **Unification** — Binding output arguments

For frequently-called predicates, this overhead can be significant compared to the actual operation.

### Optimization Strategies

**1. Inline Critical Operations:**

Some operations are too performance-critical to implement as built-ins. These are compiled inline by the PAM compiler:

- Unification (handled by `UNIFY_*` instructions)
- Arithmetic comparison (when both arguments are known integers)
- List operations (using `GET_LIST`, `PUT_LIST`)

**2. Specialized Instructions:**

The PAM instruction set includes specialized variants of common operations:

```c
// opcodes.h:150-152
Item(INT_COMPARE_REG_REG)    // Compare two registers (integers)
Item(INT_COMPARE_REG_INT)    // Compare register to constant
Item(EQ_REG_REG)             // Equality test
```

These execute the operation **without** the overhead of calling a C function.

**3. Type-Specific Paths:**

Arithmetic evaluation has fast paths for common cases:

```c
// is.c:59-82 (from earlier)
if (IsNUM(tmp1) && IsNUM(tmp2)) {
  // FAST PATH: Both small integers
  sword a = GetSmall(tmp1);
  sword b = GetSmall(tmp2);
  MakeIntegerTerm(*val, (a + b));
  return;
}

if (IsFLT(tmp1) || IsFLT(tmp2)) {
  // MEDIUM PATH: Floating point
  double a = get_float(tmp1);
  double b = get_float(tmp2);
  MakeFloatTerm(*val, (a + b));
  return;
}

// SLOW PATH: Bignum arithmetic
*val = bignum_add(tmp1, tmp2);
```

**4. Caching:**

Frequently-accessed global data is cached:

```c
// builtin.c:1626-1628
void initialize_builtin() {
  term_list = TagAtm(store_atom("."));  // Cache '.' atom
  // ... predicates use term_list directly
}
```

## Writing New Built-in Predicates

Adding a new built-in predicate follows a standard recipe.

### Step 1: Implement C Function

```c
// mymodule.c
#include "include.h"
#include "term.h"
#include "exstate.h"
#include "unify.h"
#include "error.h"

bool akl_my_predicate(Arg)
    Argdecl;
{
  Term X0, X1, result;

  // Dereference arguments
  Deref(X0, A(0));
  Deref(X1, A(1));

  // Type check with suspension
  IfVarSuspend(X0);
  if (!IsATM(X0)) {
    DOMAIN_ERROR("my_predicate", 2, 1, "atom")
  }

  // Perform operation
  result = compute_result(X0);

  // Unify result
  return unify(X1, result, exs->andb, exs);
}
```

### Step 2: Create Initialization Function

```c
// mymodule.c
void initialize_mymodule() {
  define("my_predicate", akl_my_predicate, 2);
  define("another_predicate", akl_another_predicate, 3);
  // ... register all predicates in module
}
```

### Step 3: Add to Startup Sequence

```c
// initial.c
static void init_intrinsics() {
  // ... existing initializations ...
  initialize_mymodule();      // ADD THIS LINE
  // ... remaining initializations ...
}
```

### Step 4: Recompile Emulator

```bash
cd emulator
make clean
make
```

The new predicate is now available to all AKL programs.

## Comparison with WAM Built-ins

The AGENTS built-in system differs from WAM (Warren Abstract Machine) Prolog in several ways:

| Aspect | WAM Prolog | AGENTS |
|--------|-----------|--------|
| **Suspension** | Not supported | Type-checkers suspend on variables |
| **Concurrency** | Single-threaded | Thread-safe for concurrent agents |
| **CVA Integration** | No equivalent | Ports, streams, reflection use CVAs |
| **Error Handling** | Exception-based | Error term propagation |
| **Arithmetic** | Limited to small integers | Bignum support built-in |
| **Reflection** | Limited | Extensive (execution state capture) |
| **Foreign Interface** | Implementation-specific | Standardized foreign/3 |

The suspension integration is particularly significant. In Prolog:

```prolog
?- integer(X), X = 5.
fail  % integer(X) fails immediately on variable
```

In AKL:

```prolog
?- integer(X) |  X = 5.
true  % integer(X) suspends, wakes when X=5, succeeds
```

This allows built-ins to participate in constraint propagation and concurrent execution.

## Practical Examples

### Example 1: Type-Safe Arithmetic

```prolog
% Define safe division that checks for zero
safe_div(X, Y, Z) :=
    integer(X),           % Suspend until X is integer
    integer(Y),           % Suspend until Y is integer
    Y =\= 0,              % Check not zero
    Z is X // Y.

% Usage with concurrent binding
?- safe_div(X, Y, Z) | X = 10 | Y = 2.
Z = 5  % All constraints satisfied

?- safe_div(X, Y, Z) | X = 10 | Y = 0.
fail  % Y =\= 0 fails
```

### Example 2: Stream Processing

```prolog
% Read all terms from a file
read_all_terms(File, Terms) :=
    see(File),
    read_terms_loop(Terms),
    seen.

read_terms_loop([Term|Rest]) :=
    read(Term),
    Term \= end_of_file,
    !,
    read_terms_loop(Rest).
read_terms_loop([]).

% Usage
?- read_all_terms('data.pl', Terms).
Terms = [foo(1), bar(2), baz(3)]
```

### Example 3: Performance Monitoring

```prolog
% Time a goal's execution
time_goal(Goal, Time) :=
    statistics(runtime, [_,_]),           % Reset timer
    call(Goal),                           % Execute goal
    statistics(runtime, [_,Time]).        % Get elapsed time

% Usage
?- time_goal(append([1,2,3], [4,5,6], L), T).
L = [1,2,3,4,5,6],
T = 3  % 3 milliseconds
```

### Example 4: Dynamic Functor Construction

```prolog
% Build structure with variable arity
make_structure(Name, Args, Structure) :=
    length(Args, Arity),
    name_arity_to_functor(Name, Arity, Functor),
    functor_to_tree(Functor, Structure),
    fill_args(Args, Structure, 1).

fill_args([], _, _).
fill_args([Arg|Args], Structure, N) :=
    arg(N, Structure, Arg),
    N1 is N + 1,
    fill_args(Args, Structure, N1).

% Usage
?- make_structure(foo, [a,b,c], S).
S = foo(a,b,c)

?- make_structure(bar, [1,2], S).
S = bar(1,2)
```

## Summary

The built-in predicate system provides the essential bridge between AKL's high-level constraint logic programming and low-level system capabilities. Key design principles include:

1. **Uniform calling convention** using `Argdecl`, `Deref`, and return values
2. **Suspension integration** via `IfVarSuspend` for constraint programming
3. **Modular organization** with separate implementation files per domain
4. **Systematic initialization** through `initialize_*` functions
5. **Consistent error handling** using standardized error macros
6. **Performance optimization** through type-specific fast paths

The system encompasses approximately 134 predicates across 22 modules:

- **Term testing and manipulation** (type checking, functor operations, structure access)
- **Arithmetic** (evaluation, comparison, transcendental functions)
- **I/O** (file operations, term reading/writing)
- **Reflection** (execution state introspection, solution collection)
- **Debugging** (tracing, spy points, execution monitoring)
- **Statistics** (runtime metrics, GC information)
- **Ports** (concurrent communication)
- **Foreign interface** (external C function calls)
- **Specialized structures** (arrays, hash tables, rationals)

The integration with AKL's suspension mechanism is particularly powerful, allowing type-checking predicates to participate in constraint propagation and concurrent execution. This distinguishes AGENTS from traditional Prolog systems and enables the constraint logic programming paradigm that AKL embodies.

In the next chapter, we'll examine the **AKL compiler** that translates high-level AKL source code into the PAM bytecode that the emulator executes, including how it optimizes predicate calls and integrates with the built-in system.
