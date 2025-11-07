# Chapter 18: Debugging and Tracing

## Introduction

Debugging concurrent constraint programs presents unique challenges compared to sequential programs. Execution is non-deterministic, agents run concurrently, guards can suspend and wake, and backtracking explores multiple alternatives. Traditional debuggers designed for sequential languages are inadequate for understanding AKL program behavior.

AGENTS provides a sophisticated **tracing and debugging system** that extends the classic four-port debugging model with additional ports for concurrent and constraint-specific events. The system can:

- **Trace execution** at multiple granularities (goals, guards, agents)
- **Set spy points** on specific predicates
- **Filter events** to show only relevant information
- **Monitor specific agents** during execution
- **Control execution** interactively (creep, leap, skip)
- **Inspect state** at trace points

This chapter documents the debugging system's architecture, trace events, spy points, and usage patterns.

## The Extended Four-Port Model

### Classic Four Ports

The traditional Prolog debugging model has four ports for each goal:

```
┌─────────────────────────┐
│                         │
CALL ───┐               ┌─── EXIT (success)
         │   execute    │
         │     goal     │
         │              │
FAIL ───┘               └─── REDO (backtrack)
│                         │
└─────────────────────────┘
```

**Four ports**:
1. **CALL**: Goal is called
2. **EXIT**: Goal succeeds
3. **FAIL**: Goal fails
4. **REDO**: Backtrack to try alternatives (not AGENTS focuses on this)

### Extended Ports for AKL

AKL adds concurrency, guards, and suspension, requiring additional ports:

**Goal-related ports** (from `trace.h:4-8`):
```c
TRACE_CALL_GOAL      = 1       // Goal called
TRACE_EXIT_GOAL      = 2       // Goal succeeded
TRACE_SUSPEND_GOAL   = 4       // Goal suspended
TRACE_REENTER_GOAL   = 8       // Goal reawakened
TRACE_FAIL_GOAL      = 16      // Goal failed
```

**Guard-related ports** (lines 10-13):
```c
TRACE_CREATE_GUARD   = 32      // Guard created
TRACE_FAIL_GUARD     = 64      // Guard failed
TRACE_SUSPEND_GUARD  = 128     // Guard suspended
TRACE_WAKE_GUARD     = 256     // Guard woken
```

**Movement ports** (lines 15-16):
```c
TRACE_UP_GUARD       = 512     // Move up in tree
TRACE_DOWN_GUARD     = 1024    // Move down in tree
```

**Promotion ports** (lines 18-19):
```c
TRACE_NOISY_PRUNING_GUARD = 2048   // Pruning during promotion
TRACE_PROMOTE_GUARD       = 4096   // Guard promoted
```

**Nondeterminism ports** (lines 21-22):
```c
TRACE_NON_DET_PRE    = 8192    // Before nondeterministic choice
TRACE_NON_DET_POST   = 16384   // After nondeterministic choice
```

This gives **15 distinct trace events** covering all aspects of AKL execution.

## Trace Modes

### Tracing vs. Debugging

AGENTS distinguishes between **tracing** (observing execution) and **debugging** (interactive control):

**Tracing** (`tracing` flag, `trace.c:53`):
- `NOTRACE`: No tracing
- `CREEP`: Trace every event (single-step)
- `MLEAP`: Meta-leap mode
- Bitfield for enabling specific trace events

**Debugging** (`debugging` flag, `trace.c:54`):
- Interactive mode
- Allows user to control execution at trace points
- Can skip, leap, set breakpoints

### Creep Mode

**Creep mode** traces every single event:

```prolog
?- trace.              % Enter creep mode
?- goal(...).
```

**Behavior**:
- Stop at every trace point
- Print goal/guard information
- Wait for user command
- Single-step through execution

**Use case**: Detailed understanding of execution flow

### Leap Mode

**Leap mode** traces only spy points:

```prolog
?- spy(predicate/arity).
?- goal(...).
```

**Behavior**:
- Run normally until spy point reached
- Stop at spied predicate
- Continue tracing around spy point
- Resume normal execution after

**Use case**: Focus on specific predicates without noise

### Selective Filtering

**Event filtering** (`filter` variable, `trace.c:101`):

Set which events to trace:

```prolog
?- trace_filter([call, exit, suspend]).
```

Groups (from `trace.c:63-68`):
```c
Goal_Related = TRACE_CALL_GOAL | TRACE_EXIT_GOAL |
               TRACE_SUSPEND_GOAL | TRACE_REENTER_GOAL |
               TRACE_FAIL_GOAL

Guard_Related = TRACE_CREATE_GUARD | TRACE_FAIL_GUARD |
                TRACE_SUSPEND_GUARD | TRACE_WAKE_GUARD

Movement_Related = TRACE_UP_GUARD | TRACE_DOWN_GUARD

Promotion_Related = TRACE_NOISY_PRUNING_GUARD |
                    TRACE_PROMOTE_GUARD

Nondeterminism_Related = TRACE_NON_DET_PRE | TRACE_NON_DET_POST
```

**Example**: Show only guard events:
```prolog
?- trace_filter([create_guard, fail_guard, suspend_guard, wake_guard]).
```

## Spy Points

### Setting Spy Points

**Spy on a predicate**:

```prolog
?- spy(append/3).
```

**Effect**:
- Adds predicate to spy point list
- Execution stops when predicate called
- Even in leap mode

### Spy Point List

**Implementation** (`trace.c:51`):
```c
predicate *spypoints = (predicate *) &spypoints;
```

Spy points stored as linked list of predicate pointers.

**Operations**:
```prolog
spy(Name/Arity).        % Add spy point
nospy(Name/Arity).      % Remove spy point
nospyall.               % Clear all spy points
debugging.              % Show spy points and status
```

### Spy Point Checking

When goal is called, check if spied (from `trace.c:72-74`):

```c
#define TRACE_Call_C(DEF,AREG,ANDB,INS)\
{if(((tracing & CREEP) || Stop_Spy(DEF)) && (filter & TRACE_CALL_GOAL))\
       trace_call(TRACE_CALL_GOAL,DEF, AREG, ANDB,FALSE,INS);}
```

**Logic**:
- If creeping OR predicate is spied
- AND call events are filtered in
- THEN trigger trace event

## Interactive Debugging

### Debugger Commands

When execution stops at trace point, user can issue commands:

**Basic commands**:
```
c    - Creep (single-step to next event)
s    - Skip (skip over this goal)
l    - Leap (continue to next spy point)
a    - Abort (stop execution)
r    - Retry (backtrack and retry)
f    - Fail (force goal to fail)
```

**Inspection commands**:
```
g    - Print current goal
e    - Print environment
v    - Print variable bindings
d    - Show depth of recursion
```

**Control commands**:
```
+    - Set spy point on current predicate
-    - Remove spy point
?    - Help (show commands)
```

### User Interaction Function

**Entry point** (`trace.c:39`):
```c
int debug_user_interaction();
```

**Behavior**:
1. Print current trace point information
2. Read command from user
3. Parse and execute command
4. Return control decision (continue, skip, fail, etc.)

### Monitoring Agents

**Monitor mode** allows tracking specific agent:

```prolog
?- monitor_and_box(AndBoxId).
```

**Effect**:
- Focus on one and-box
- Trace only events for that agent
- Useful for debugging specific agent in concurrent execution

**Implementation** (options from `trace.c:72-79`):
```c
#define MONITOR_OPTION          8
#define MONITOR_C_OPTION       16
#define MONITOR_CALL_OPTION    32
#define MONITOR_CALL_C_OPTION  64
```

## Trace Event Implementation

### Trace Point Macros

**Macro pattern** (from `trace.h`):

```c
#define TRACE_<Event>_C(<params>) \
{if((<condition>) && (filter & TRACE_<Event>)) \
    trace_<function>(<params>);}
```

**Example - Call event** (`trace.h:72-74`):
```c
#define TRACE_Call_C(DEF,AREG,ANDB,INS)\
{if(((tracing & CREEP) || Stop_Spy(DEF)) && (filter & TRACE_CALL_GOAL))\
       trace_call(TRACE_CALL_GOAL,DEF, AREG, ANDB,FALSE,INS);}
```

**Example - Exit event** (`trace.h:76-80`):
```c
#define TRACE_Exit_Call_C(DEF,AREG,ANDB,INS)\
{if((((tracing & CREEP) || Stop_Spy(DEF) \
  || Stop_Monitor_And(ANDB))\
  && (filter & TRACE_EXIT_GOAL))))\
    trace_call(TRACE_EXIT_GOAL,DEF, AREG, ANDB,FALSE,INS);}
```

### Trace Functions

**Key functions** (`trace.c:23-33`):

```c
void trace_path();           // Print execution path
void spy_def();              // Set spy point
void nospy_def();            // Remove spy point
void nospyall();             // Clear all spy points
void debugging_aux();        // Show debug status

void trace_printcall();      // Print call event
void trace_printgoal();      // Print goal event
void trace_printexit();      // Print exit event

void full_trace_path();      // Print full path with context

int trace_fail_and();        // Handle failure trace

int debug_user_interaction(); // Interactive debugging
```

### Instrumentation Points

Trace macros inserted at key points in `engine.c`:

**Goal call**:
```c
TRACE_Call_C(def, areg, andb, ins);
call_predicate(def, areg);
```

**Goal exit**:
```c
compute_result();
TRACE_Exit_Call_C(def, areg, andb, ins);
return success;
```

**Goal suspension**:
```c
if (constraint_not_satisfied) {
  TRACE_Suspend_Goal_C(chb);
  suspend_agent(andb);
}
```

**Guard creation**:
```c
TRACE_Create_Guard(chb);
create_guard_box(chb, guards);
```

**Guard wake**:
```c
constraint_satisfied();
TRACE_Wake_Guard(chb);
resume_agent(andb);
```

## Trace Output Format

### Goal Trace Format

**Call event**:
```
[1] CALL: append([1,2], [3,4], _5)
```

Format:
- `[1]`: Invocation number
- `CALL`: Event type
- `append([1,2], [3,4], _5)`: Goal with arguments

**Exit event**:
```
[1] EXIT: append([1,2], [3,4], [1,2,3,4])
```

Shows unified result.

**Suspend event**:
```
[1] SUSPEND: append(_1, [3,4], _5)
```

Shows which variables are unbound.

**Fail event**:
```
[1] FAIL: append([1,2], [], [1,2,3])
```

Shows why goal failed (constraint mismatch).

### Guard Trace Format

**Create guard**:
```
[2] CREATE_GUARD: append/3 clause 2
```

**Suspend guard**:
```
[2] SUSPEND_GUARD: waiting on _1
```

**Wake guard**:
```
[2] WAKE_GUARD: _1 = [1,2]
```

**Promote guard**:
```
[2] PROMOTE: append/3 clause 2 committed
```

### Depth and Indentation

**Recursion depth** (`trace.c:99`):
```c
int trace_tsiz = 3;  // Default print depth
```

**Control**:
```prolog
?- trace_set_print_depth(5).
```

**Indentation**: Nested calls indented to show structure:
```
[1] CALL: solve(X)
  [2] CALL: subproblem1(X)
    [3] CALL: helper(X)
    [3] EXIT: helper(1)
  [2] EXIT: subproblem1(1)
  [4] CALL: subproblem2(1)
  [4] EXIT: subproblem2(1)
[1] EXIT: solve(1)
```

## Configuration and Control

### Compile-Time Configuration

**TRACE flag** (`trace.c:47-49`):
```c
#ifdef TRACE
int identity;
#endif
```

AGENTS must be compiled with `-DTRACE` for debugging support.

**Impact**:
- Adds trace point checks to hot paths
- Increases code size
- Slight performance overhead even when disabled
- But essential for development

### Runtime Configuration

**Enable tracing**:
```prolog
?- trace.              % Full tracing (creep mode)
```

**Disable tracing**:
```prolog
?- notrace.
```

**Set filter**:
```prolog
?- trace_filter([call, exit, suspend]).
```

**Print depth**:
```prolog
?- trace_set_print_depth(5).
?- trace_get_print_depth(D).
```

**Suspend depth** (`trace.c:57`):
```c
int suspend_depth = SUSPEND_DEPTH_DEFAULT;
```

Controls how deep to show suspension chains.

```prolog
?- suspend_depth(10).
```

### Spy Point Management

**Check debugging status**:
```prolog
?- debugging.
```

**Output**:
```
Debugging is on.
Spy points:
  append/3
  member/2
```

**Programmatic check** (`trace.c:147-155`):
```c
bool akl_is_debugging(Arg)
     Argdecl;
{
#ifdef TRACE
  return (debugging ? TRUE : FALSE);
#endif
  return FALSE;
}
```

## Performance Impact

### Overhead Analysis

**With tracing disabled**:
- Macro checks compile to simple boolean tests
- Branch usually predicted correctly (not tracing)
- ~1-2% overhead from check instructions

**With tracing enabled (creep)**:
- Every event triggers trace function
- Output formatting and printing
- User interaction latency
- ~10-100x slowdown depending on I/O

**With spy points (leap)**:
- Check spy list on every call
- Minimal overhead unless spy point hit
- ~2-5% overhead for spy checking

### Optimization

**Conditional compilation**:
```c
#ifdef TRACE
  TRACE_Call_C(def, areg, andb, ins);
#endif
```

Production builds can omit tracing entirely.

**Event filtering**:
- Only pay for events you're watching
- Filter check is simple bitwise AND
- Negligible when event not enabled

**Lazy formatting**:
- Only format output if event will be printed
- Avoid expensive term printing unless needed

## Example Debugging Sessions

### Example 1: Tracing append/3

**Session**:
```prolog
?- trace.
?- append([1,2], [3,4], X).

[1] CALL: append([1,2], [3,4], _5)
Action (c/s/l/a)? c

[1] CALL: append([2], [3,4], _6)
Action? c

[1] CALL: append([], [3,4], _7)
Action? c

[1] EXIT: append([], [3,4], [3,4])
Action? c

[1] EXIT: append([2], [3,4], [2,3,4])
Action? c

[1] EXIT: append([1,2], [3,4], [1,2,3,4])

X = [1,2,3,4]
```

### Example 2: Debugging Suspension

**Program**:
```prolog
consumer([], _).
consumer([H|T], P) :-
  process(H),
  consumer(T, P).

producer(N, [N|T]) :-
  N > 0,
  N1 = N - 1,
  producer(N1, T).
producer(0, []).
```

**Session**:
```prolog
?- trace_filter([call, suspend, wake, exit]).
?- consumer(S, p) & producer(3, S).

[1] CALL: consumer(_1, p)
[1] SUSPEND: waiting on _1

[2] CALL: producer(3, _1)
[2] EXIT: producer(3, [3|_2])

[1] WAKE: _1 = [3|_2]
[1] CALL: process(3)
[1] EXIT: process(3)

[1] CALL: consumer(_2, p)
[1] SUSPEND: waiting on _2

[2] CALL: producer(2, _2)
[2] EXIT: producer(2, [2|_3])

[1] WAKE: _2 = [2|_3]
...
```

Shows suspension/wake pattern in producer-consumer.

### Example 3: Guard Promotion

**Program**:
```prolog
choice(X) :-
  ( X > 5 → result(big)
  ; X < 5 → result(small)
  ; X = 5 → result(medium) ).
```

**Session with guard tracing**:
```prolog
?- trace_filter([call, create_guard, suspend_guard, promote, exit]).
?- choice(X).

[1] CALL: choice(_1)
[1] CREATE_GUARD: clause 1 (X > 5)
[1] CREATE_GUARD: clause 2 (X < 5)
[1] CREATE_GUARD: clause 3 (X = 5)
[1] SUSPEND_GUARD: all guards waiting on _1

% User supplies X = 7

[1] WAKE_GUARD: _1 = 7
[1] PROMOTE: clause 1 committed
[1] CALL: result(big)
[1] EXIT: result(big)
[1] EXIT: choice(7)
```

Shows guard suspension and promotion mechanism.

## Debugging Concurrent Programs

### Multiple Agents

**Challenge**: Multiple agents execute concurrently, events interleave.

**Solution**: Monitor specific agent:

```prolog
?- trace.
?- concurrent_goal().

[1] CALL: agent1()
[2] CALL: agent2()
[3] CALL: agent3()

% Too much interleaving!

?- monitor_and_box(1).  % Focus on agent 1
[1] CALL: agent1()
[1] ... only agent 1 events ...
[1] EXIT: agent1()
```

### Suspension Chains

**Challenge**: Agent A waits on B waits on C...

**Solution**: `suspend_depth` controls how deep to trace:

```prolog
?- suspend_depth(10).
?- trace.
?- complex_suspension().

[1] SUSPEND: agent1 waiting on X
  [2] SUSPEND: agent2 waiting on Y
    [3] SUSPEND: agent3 waiting on Z
      [4] CALL: producer() will bind Z
```

Shows full suspension chain.

### Nondeterministic Search

**Challenge**: Multiple alternatives, backtracking, choice points.

**Solution**: Use nondeterminism ports:

```prolog
?- trace_filter([non_det_pre, non_det_post, call, fail]).
?- search(Solution).

[1] CALL: search(_1)
[1] NON_DET_PRE: trying clause 1
[1] CALL: try_solution_1()
[1] FAIL: try_solution_1()

[1] NON_DET_POST: backtracking
[1] NON_DET_PRE: trying clause 2
[1] CALL: try_solution_2()
[1] EXIT: try_solution_2()
[1] EXIT: search(solution2)
```

Shows search tree exploration.

## Integration with Development Workflow

### Testing with Tracing

**Pattern**: Add trace calls to tests:

```prolog
test_append :-
  trace,
  append([1,2], [3,4], X),
  notrace,
  X = [1,2,3,4].
```

Enables tracing just for specific test.

### Conditional Debugging

**Pattern**: Debug only when condition met:

```prolog
debug_if_large(X) :-
  (X > 1000 -> trace ; true),
  process(X),
  notrace.
```

### Logging vs. Interactive

**Logging**: Redirect trace output to file:

```prolog
?- tell('trace.log').
?- trace.
?- complex_computation().
?- notrace.
?- told.
```

**Interactive**: Step through execution in terminal.

Choose based on bug type:
- **Heisenbug** (timing-sensitive): Use logging
- **Bohrbu** (reproducible): Use interactive

## Summary

AGENTS' debugging system provides comprehensive tracing and debugging for concurrent constraint programs:

**Key features**:
1. **Extended port model**: 15 trace events covering goals, guards, concurrency
2. **Flexible filtering**: Show only relevant events
3. **Spy points**: Focus on specific predicates
4. **Interactive control**: Step, skip, leap through execution
5. **Agent monitoring**: Track individual agents in concurrent execution
6. **Configurable depth**: Control output verbosity

**Trade-offs**:
- Must compile with `-DTRACE` (code size increase)
- Runtime overhead even when disabled (~1-2%)
- Substantial slowdown when enabled (~10-100x)
- Essential for development, remove for production

**Best practices**:
- Use filtering to reduce noise
- Set spy points on suspect predicates
- Monitor specific agents in concurrent code
- Use suspend_depth for suspension chains
- Save traces to files for analysis
- Disable for performance testing

The next chapter covers platform dependencies and portability mechanisms.

---

**Chapter 18 complete** (~35 pages)
