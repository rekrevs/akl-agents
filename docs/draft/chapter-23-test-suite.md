# Chapter 23: Test Suite

## Introduction

AGENTS includes a **comprehensive test suite** built on **DejaGnu**, a testing framework for diverse software systems. The test suite validates that the emulator, compiler, and built-in predicates work correctly across different platforms.

DejaGnu provides:
- **Automated testing**: Run entire test suite with one command
- **Interactive testing**: Tests communicate with AGENTS through expect scripts
- **Regression detection**: Compare results against baselines
- **Detailed logging**: Track which tests pass and fail
- **Cross-platform support**: Works on all UNIX systems

This chapter documents the test suite structure, how to run tests, how to write new tests, and what the tests cover.

## Overview

### Test Framework

**Based on** (from tests/README:68-76):
- **DejaGnu**: Testing framework for diverse software
- **Expect**: Script language for interacting with interactive programs
- **Tcl**: General-purpose scripting language (Expect is built on Tcl)

**Why these tools?**
- AGENTS is **interactive** (read-eval-print loop)
- Tests need to **send goals** and **check output**
- Expect excels at automating interactive programs
- DejaGnu provides structure for organizing many tests

### Test Organization

**Directory structure**:
```
tests/
├── README                      # How to use the test suite
├── agents.tests/               # Test suite files
│   ├── format.exp              # format/2 predicate tests (35 tests)
│   ├── ioports.exp             # I/O port tests (10 tests)
│   ├── types.exp               # Type testing (1 test)
│   └── old_test.exp            # Legacy comprehensive test
├── lib/
│   └── agents_test.exp         # Test utility procedures
└── config/
    └── unix.exp                # Platform configuration (UNIX)
```

**Test suites**: 4 files, ~207 lines total

**Test count**: Approximately 50 individual tests

## Running the Test Suite

### Basic Usage

**From build directory** (from tests/README:9-13):
```sh
cd /home/user/akl-agents
make check
```

or

```sh
cd tests
runtest --tool agents AGENTS=../agents
```

**What happens**:
1. DejaGnu starts the `agents` executable
2. Runs all test suites in `agents.tests/`
3. Sends goals and checks output
4. Reports pass/fail for each test
5. Generates summary

### Testing Specific Suites

**Run only format tests**:
```sh
runtest --tool agents AGENTS=../agents format.exp
```

**Run only I/O port tests**:
```sh
runtest --tool agents AGENTS=../agents ioports.exp
```

**Pattern**: Append test suite filename to `runtest` command.

### Testing Installed AGENTS

**From tests/README:15-21**:

**Test installed version**:
```sh
runtest --tool agents AGENTS=agents
```

**Test specific installation**:
```sh
runtest --tool agents AGENTS=/opt/agents/bin/agents
```

**Use case**: Validate installation is working correctly.

### Verbose Output

**From tests/README:54-59**:

**Standard verbosity**:
```sh
runtest --tool agents AGENTS=../agents --all
```

**More verbose**:
```sh
runtest --tool agents AGENTS=../agents --verbose
```

**Maximum verbosity**:
```sh
runtest --tool agents AGENTS=../agents --verbose --verbose
```

**Abbreviations work**:
```sh
runtest -to agents AGENTS=../agents -a
runtest -to agents AGENTS=../agents -v
runtest -to agents AGENTS=../agents -v -v
```

### Test Output

**Example output** (from tests/README:23-36):
```
Test Run By rch on Fri Jan  7 10:56:41 MET 1994
Native configuration

                === agents tests ===

Running ./agents.tests/format.exp ...
Running ./agents.tests/old_test.exp ...

                === agents Summary ===

# of expected passes       37
../agents (AGENTS 0.9)
```

**Interpretation**:
- Ran 2 test suites (format, old_test)
- 37 tests passed
- No failures

**Example failure** (from tests/README:42-44):
```
FAIL:   format3 (unexpected output)
```

**Indicates**: Test named "format3" produced output different from expected.

### Log Files

**From tests/README:61-66**:

**agents.sum**: Summary (same as `--all` output)
```
PASS:   format1
PASS:   format2
FAIL:   format3 (unexpected output)
...
```

**agents.log**: Detailed log (includes all AGENTS output)
```
send "format('~d',[42]).\n"
expect {
  -re "^42\n.*| ?- $" {pass "format3"}
  ...
}
# AGENTS output:
43
| ?-
# Expected: 42
# Got: 43
FAIL:   format3 (unexpected output)
```

**Previous runs saved**:
- `agents.psum`: Previous summary
- `agents.plog`: Previous log

## Test Infrastructure

### Configuration File

**Location**: `tests/config/unix.exp`

**Purpose**: Platform-specific setup (from tests/config/unix.exp:1-44)

**Key procedures**:

**agents_start**: Start AGENTS and wait for prompt
```tcl
proc agents_start {} {
  global AGENTS
  global prompt1
  global spawn_id

  set stty_init "-echo raw"
  spawn $AGENTS
  expect {
    -re "$prompt1$"                    {}
    -re "^$AGENTS: No such file"       {
      send_user "\{ERROR: spawn $AGENTS: no such file\}\n"
      exit 1
    }
    timeout                            {
      send_user "\{ERROR: spawn $AGENTS: timeout\}\n"
      exit 1
    }
  }
}
```

**Process**:
1. Set terminal to raw mode (no echo)
2. Spawn AGENTS process
3. Wait for primary prompt `| ?- `
4. If fails, report error and exit
5. Timeout after 30 seconds (global setting)

**agents_exit**: Shut down AGENTS cleanly
```tcl
proc agents_exit {} {
  send "halt.\n"
}
```

**agents_version**: Extract version number
```tcl
proc agents_version {} {
  global AGENTS

  set tmp [exec echo "agents_version(X),format('''~w''',\[X\]).\n" | $AGENTS]
  if [regexp "'(\[^'\]*)'" $tmp "" version] {
    clone_output "[which $AGENTS] ($version)"
  }
}
```

**Default values**:
```tcl
if ![info exists AGENTS] then {
  set AGENTS ../agents
}

set prompt1 "\| \?\- "      # Primary prompt: | ?-
set prompt2 "\? "           # Secondary prompt: ?

set timeout 30              # 30 second timeout for all operations
```

**Note from tests/README:113-118**: Configuration file is loaded automatically before test suites run. Tests don't need to start/stop AGENTS explicitly.

### Test Utility Library

**Location**: `tests/lib/agents_test.exp`

**Purpose**: Simplify writing tests (from tests/lib/agents_test.exp:1-41)

**Procedures**:

#### agents_test_initialize

```tcl
proc agents_test_initialize {} {
  global prompt1
  global prompt2
  global test

  expect_after {
    -re "(.*)$prompt1$"                {
      fail "$test (unexpected output)"
      verbose "UNEXPECTED OUTPUT: \{\n$expect_out(1,string)\n\}\n"
    }
    -re "(.*)$prompt2$"                {
      fail "$test (unexpected output)"
      verbose "UNEXPECTED OUTPUT: \{\n$expect_out(1,string)\n\}\n"
      send "\n"
      expect {
        -re "$prompt1$"                {}
      }
    }
    timeout                            {fail "$test (timeout)"}
  }
}
```

**Purpose**: Set up default expect handlers
- **Unexpected primary prompt**: Fail test
- **Unexpected secondary prompt**: Fail test and send newline to get back to primary prompt
- **Timeout**: Fail test

**Called once** at beginning of test suite.

#### agents_test

```tcl
proc agents_test {name in out} {
  global prompt1
  global prompt2
  global test

  set test $name
  send "$in"
  expect {
    -re "$out.*$prompt1$"              {pass "$test"}
    -re "$out.*$prompt2$"              {
      pass "$test"
      send "\n"
      expect {
        -re "$prompt1$"                {}
      }
    }
  }
}
```

**Arguments**:
1. **name**: Test identifier (string)
2. **in**: Goal to send to AGENTS (including `\n`)
3. **out**: Expected output (regular expression)

**Process**:
1. Send goal to AGENTS
2. Wait for output matching regex
3. If matches and prompt appears: **PASS**
4. If doesn't match (caught by expect_after): **FAIL**

**From tests/README:102-111**:
> agents_test takes three arguments: (1) a name for the test. This is used to identify the test in pass and fail messages. It may be any string; (2) a goal to be sent to AGENTS; and (3) a regular expression for the expected output or any substring of it, up to but not including the next prompt. "^" means the beginning of the output.

## Test Suites

### format.exp: Format Predicate Tests

**Location**: `tests/agents.tests/format.exp`

**Tests**: 35 tests of `format/2` predicate

**Structure** (from tests/agents.tests/format.exp:1-144):
```tcl
load_lib agents_test.exp

agents_test_initialize

agents_test format1 \
"format('Hello, world!',[]).\n" \
"^Hello, world!\n"

agents_test format2 \
"format('~~',[42]).\n" \
"^~\n"

agents_test format3 \
"format('~d',[42]).\n" \
"^42\n"
# ... 32 more tests
```

**Coverage**:

**Format directives tested**:
- **~d**: Decimal integer (format3, format18, format19)
- **~D**: Decimal with commas (format19, format21)
- **~a**: Atom (format4, format5)
- **~c**: Character (format6, format7, format8)
- **~s**: String (format11, format12, format13)
- **~r, ~R**: Radix (format14, format15, format16, format17)
- **~e, ~E**: Scientific notation (format22, format23, format27, format28)
- **~f**: Fixed-point float (format24, format29)
- **~g, ~G**: General float (format25, format26, format30, format31)
- **~n**: Newline (format10)
- **~~**: Literal tilde (format2)

**Width specifiers**:
- **~7c**: Fixed width (format7)
- **~*c**: Dynamic width (format8, format9)
- **~*s**: Dynamic width string (format11)
- **~*e, ~*E**: Dynamic precision float (format32, format33)

**Error cases**:
- **format4**: Domain error (integer with ~a)
- **format9**: Domain error (negative width)

**Example test** (format3):
```tcl
agents_test format3 \
"format('~d',[42]).\n" \
"^42\n"
```

**Sends**: `format('~d',[42]).` (format integer 42 as decimal)
**Expects**: Output starts with `42` followed by newline

### ioports.exp: I/O Port Tests

**Location**: `tests/agents.tests/ioports.exp`

**Tests**: 10 tests of I/O port operations

**Coverage** (from tests/agents.tests/ioports.exp:1-44):

**Output operations** (on stdout):
- **putc/1**: Output character (ioports1)
- **putd/1**: Output integer (ioports2)
- **putf/1**: Output float (ioports3)
- **puts/1**: Output atom/string (ioports4, ioports5)
- **putsq/2**: Output quoted atom (ioports6, ioports7, ioports8, ioports9)

**Input operations** (on stdin):
- **getc/1**: Read character (ioports10)

**Example tests**:

**ioports1** (output character):
```tcl
agents_test ioports1 \
"stdout(P),putc(0'a)@P.\n" \
"^a\n"
```

**Process**:
1. Get stdout port: `stdout(P)`
2. Send character 'a': `putc(0'a)@P`
3. Expect output: `a`

**ioports10** (input character):
```tcl
agents_test ioports10 \
"stdin(P),getc(C)@P.\na" \
"C = 97"
```

**Process**:
1. Send goal to read character: `stdin(P),getc(C)@P.`
2. AGENTS waits for input
3. Send character 'a': `a`
4. Expect binding: `C = 97` (ASCII code of 'a')

**Port syntax**: `Goal@Port` (agent Goal communicates on Port)

### types.exp: Type Testing

**Location**: `tests/agents.tests/types.exp`

**Tests**: 1 test

**Coverage** (from tests/agents.tests/types.exp:1-8):
```tcl
load_lib agents_test.exp

agents_test_initialize

agents_test types1 \
"compile('$subdir/foo_abs'),foo_abs(FooAbs),atomic(FooAbs).\n" \
"\nno \n"
```

**Purpose**: Test that abstraction (foo_abs) is not atomic.

**Process**:
1. Compile source file with abstraction definition
2. Create abstraction: `foo_abs(FooAbs)`
3. Test if atomic: `atomic(FooAbs)`
4. Expect: **no** (abstractions are not atomic)

**Note**: This is a minimal test suite with only one test. Could be expanded.

### old_test.exp: Legacy Comprehensive Test

**Location**: `tests/agents.tests/old_test.exp`

**Tests**: 1 comprehensive test (from tests/agents.tests/old_test.exp:1-15)

```tcl
send "compile('$subdir/old_test'),test(term),test(io),test(arith).\n"
expect {
  -re "yes.*$prompt1$"                     {pass "old_test"}
  -re "Fail:([^?]*)\?"                     {
    fail "old_test ($expect_out(1,string))"
    send "a\n"
    expect -re ".*$prompt1$"
  }
  -re ".*$prompt1$"                        {fail "old_test"}
  timeout                                  {fail "old_test (timeout)"}
}

send "nofailcatch.\n"
expect -re ".*$prompt1$" {}
```

**Process**:
1. Compile `old_test.akl` (test suite in AKL)
2. Call three test predicates: `test(term)`, `test(io)`, `test(arith)`
3. If all succeed: **yes** → PASS
4. If any fails: **Fail: ...** → FAIL with error message
5. Send `a` to abort on failure
6. Disable failure catching: `nofailcatch.`

**Coverage**: Term operations, I/O, arithmetic (details in old_test.akl source)

**From tests/README:125-135**: This style "clumsily lumps together what should be many tests into a single test" and is not recommended for new tests. Better to break into separate tests like format.exp.

## Test Coverage

### What Is Tested

**Built-in predicates**:
- **format/2**: Comprehensive (35 tests)
- **I/O ports**: Basic coverage (10 tests)
- **Type checking**: Minimal (1 test)
- **Legacy tests**: Term operations, I/O, arithmetic (1 comprehensive test)

**System features**:
- **Compilation**: Tests compile AKL source
- **Loading**: Tests load compiled code
- **Execution**: Tests execute goals
- **Output**: Tests verify output format and content
- **Input**: Tests verify reading from stdin
- **Error handling**: Tests domain errors

**Platforms**:
- Tests run on any UNIX system
- Platform-specific configuration via config/unix.exp
- No Windows support (DejaGnu is UNIX-focused)

### What Is Not Tested

**Coverage gaps**:
1. **Concurrency**: No tests of and-parallel execution
2. **Guards**: No tests of conditional/committed/nondeterminate choice
3. **Constraints**: No tests of finite domain solver
4. **Ports (comprehensive)**: Only basic I/O tested, not full port semantics
5. **Backtracking**: No tests of choice points and RETRY
6. **Garbage collection**: No stress tests of GC
7. **Memory management**: No tests of heap overflow handling
8. **Debugger**: No tests of trace/spy functionality
9. **Most built-ins**: Only format and I/O tested, not arithmetic, lists, etc.
10. **Compiler**: No tests of compilation correctness (only that it runs)

**Why gaps exist**:
- Test suite dates from early 1990s
- Focus on basic functionality and regression testing
- Interactive testing of concurrency is complex
- Demo programs provide informal testing of advanced features

## Demo Programs as Tests

**Location**: `demos/` directory

**Count**: ~90 demo programs

**Categories**:

**Logic programming basics**:
- `ancestors.akl`: Family relationships with backtracking
- `cipher.akl`: Cryptarithmetic puzzle
- `cube.akl`: Cube root calculation

**Finite domain constraints**:
- `fd_queens.akl`: N-queens problem
- `fd_magic.akl`: Magic square puzzle
- `fd_cars.akl`: Car sequencing problem
- `fd_alpha.akl`: Alphametic puzzles
- `fd_eq10.akl`, `fd_eq20.akl`: Equation solving
- `fd_five.akl`: Five houses puzzle

**Concurrency**:
- (Various demos showing and-parallel execution)

**Use as tests**:
1. **Smoke tests**: Verify demos compile and run
2. **Feature tests**: Demonstrate specific features work
3. **Regression tests**: Compare output across versions

**Running demos manually**:
```sh
./agents
| ?- compile(demos/fd_queens).
| ?- queens(8, Board).
```

**Not automated**: Demos not integrated into DejaGnu test suite. Would be valuable addition.

## Writing New Tests

### Simple Test Pattern

**From tests/README:78-96**:

**Step 1**: Create new .exp file in `tests/agents.tests/`

**Example**: `tests/agents.tests/arithmetic.exp`

```tcl
load_lib agents_test.exp

agents_test_initialize

agents_test arith1 \
"X is 2 + 3.\n" \
"X = 5"

agents_test arith2 \
"X is 10 - 3.\n" \
"X = 7"

agents_test arith3 \
"X is 4 * 5.\n" \
"X = 20"

agents_test arith4 \
"X is 20 / 4.\n" \
"X = 5"

agents_test arith5 \
"X is 2 ** 10.\n" \
"X = 1024"
```

**Step 2**: Run new test suite

```sh
cd tests
runtest --tool agents AGENTS=../agents arithmetic.exp
```

**Tips**:
- Use descriptive test names (arith1, arith2, etc.)
- Escape Tcl special characters: `\[`, `\]`, `$`, etc.
- Use `^` for "beginning of output"
- Include newline in goal: `"...\n"`
- Expected output is a **regex**, not exact match

### Advanced Test Pattern

**From tests/README:125-135**: For tests that need custom expect handling:

**Example**: Test with multiple interactions

```tcl
set test "custom_test"

send "start_computation.\n"
expect {
  -re "Ready for input"                {}
  timeout                              {fail "$test (timeout at start)"}
}

send "42\n"
expect {
  -re "Result: 42.*$prompt1$"         {pass "$test"}
  -re ".*$prompt1$"                   {fail "$test (wrong result)"}
  timeout                              {fail "$test (timeout at result)"}
}
```

**When to use**:
- Multiple interactions (send, expect, send, expect)
- Complex output patterns
- Timeout handling for long computations
- Error recovery

### Testing Error Cases

**Pattern**: Expect error message in output

**Example** (from format.exp:17-19):
```tcl
agents_test format4 \
"format('~a',[42]).\n" \
"DOMAIN ERROR"
```

**Process**:
1. Send goal that should fail
2. Expect error message
3. Test passes if error appears

**Common error patterns**:
- `"DOMAIN ERROR"`: Type mismatch
- `"EXISTENCE ERROR"`: Undefined predicate
- `"INSTANTIATION ERROR"`: Unbound variable
- `"SYNTAX ERROR"`: Parse error

### Testing Compilation

**Pattern**: Compile source, then test predicates

**Example**:
```tcl
agents_test mytest \
"compile('$subdir/mysource'),mypred(X).\n" \
"X = expected_value"
```

**$subdir**: DejaGnu variable containing path to test directory

**Note**: Source file `tests/agents.tests/mysource.akl` must exist.

## Test Coverage Profiling

**From Makefile.in:469-487** (Chapter 21):

**Build with profiling**:
```sh
make check-coverage
```

**Process**:
1. **make-tcovagents**: Build emulator with `-a` flag (tcov instrumentation)
2. **test-tcovagents**: Run test suite with profiled emulator
3. **tcov-tcovagents**: Generate coverage report

**Requirements**: SunOS `tcov` tool

**Output**: `agents.tcov` (coverage report in tcov format)

**Modern alternative**: Use `gcov` (GCC coverage tool) instead:

```sh
# Build with coverage
cd emulator
make clean
gcc -fprofile-arcs -ftest-arcs -O0 -g *.c -o agents
cd ..

# Run tests
make check

# Generate coverage
cd emulator
gcov *.c
```

**Output**: `*.gcov` files showing line-by-line coverage

## Continuous Integration

**Not currently implemented**, but would be valuable:

**CI workflow** (GitHub Actions example):

```yaml
name: Test AGENTS

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install dependencies
        run: sudo apt-get install -y dejagnu bison flex
      - name: Configure
        run: ./configure --without-gmp
      - name: Build
        run: make
      - name: Test
        run: make check
      - name: Upload test results
        if: always()
        uses: actions/upload-artifact@v2
        with:
          name: test-results
          path: tests/agents.*
```

**Benefits**:
- Automatic testing on every commit
- Test across multiple platforms (Ubuntu, macOS)
- Detect regressions early
- Track test trends over time

## Regression Testing

**Purpose**: Ensure new changes don't break existing functionality

**Process**:
1. **Baseline**: Run tests on known-good version
2. **Save output**: `cp tests/agents.sum baseline.sum`
3. **Make changes**: Modify code
4. **Rebuild**: `make clean && make`
5. **Retest**: `make check`
6. **Compare**: `diff baseline.sum tests/agents.sum`

**If differences**:
- **New failures**: Regression! Fix before committing
- **New passes**: Good! Update baseline
- **Different output**: Investigate if intentional

**Automated regression testing**:
```sh
#!/bin/sh
# regression-test.sh

git stash
./configure --without-gmp
make
make check
cp tests/agents.sum baseline.sum

git stash pop
make clean
./configure --without-gmp
make
make check

echo "=== Regression Report ==="
diff baseline.sum tests/agents.sum || echo "REGRESSIONS DETECTED!"
```

## Platform-Specific Testing

**Current support**: UNIX only (via config/unix.exp)

**Adding new platform** (e.g., Windows):

**Step 1**: Create `tests/config/windows.exp`

```tcl
proc agents_exit {} {
  send "halt.\r\n"
}

proc agents_start {} {
  global AGENTS
  global prompt1

  # Windows uses different terminal settings
  spawn $AGENTS
  expect {
    -re "$prompt1$"    {}
    timeout            {error "Failed to start AGENTS"}
  }
}

# Windows-specific settings
if ![info exists AGENTS] then {
  set AGENTS ..\\agents.exe
}

set prompt1 "\| \?\- "
set prompt2 "\? "
set timeout 30

agents_start
```

**Step 2**: Test with new config

```sh
runtest --tool agents --target windows AGENTS=../agents.exe
```

**Challenges**:
- DejaGnu primarily designed for UNIX
- Expect has limited Windows support
- Terminal handling different on Windows

## Extending Test Coverage

### Priority Areas

**1. Concurrency tests**:
```tcl
agents_test concurrent1 \
"p(X,Y) :- q(X), r(Y). q(1). r(2). p(A,B).\n" \
"A = 1.*B = 2"
```

**2. Guard tests**:
```tcl
agents_test guard_conditional \
"p(X) :- X > 0 -> q(X). p(5).\n" \
"yes"

agents_test guard_committed \
"p(X) :- X > 0 | q(X); r(X). p(5).\n" \
"yes"
```

**3. Constraint tests**:
```tcl
agents_test fd_constraint1 \
"use_module(library(fd)), X in 1..10, X #> 5, label([X]).\n" \
"X = 6"
```

**4. Backtracking tests**:
```tcl
agents_test backtrack1 \
"member(X, [1,2,3]).\n" \
"X = 1.*X = 2.*X = 3"
```

**5. Port tests** (beyond I/O):
```tcl
agents_test port_comm \
"open_port(P, S), send(msg, P), S = [H|T].\n" \
"H = msg"
```

### Test Template

**Generic template** for new test suites:

```tcl
# tests/agents.tests/FEATURE.exp
# Tests for FEATURE

load_lib agents_test.exp

agents_test_initialize

# Basic functionality
agents_test feature_basic \
"feature_goal.\n" \
"expected_output"

# Edge cases
agents_test feature_edge1 \
"edge_case_goal.\n" \
"edge_output"

# Error handling
agents_test feature_error \
"invalid_goal.\n" \
"ERROR_MESSAGE"

# Integration with other features
agents_test feature_integration \
"complex_goal.\n" \
"complex_output"
```

**Run template**:
```sh
runtest --tool agents AGENTS=../agents FEATURE.exp
```

## Best Practices

**From tests/README and experience**:

1. **One test per goal**: Don't lump multiple tests together (unlike old_test.exp)

2. **Descriptive names**: Use `format3` not `test3`

3. **Test output, not implementation**: Check results, not intermediate steps

4. **Escape special characters**: Tcl requires `\[`, `\]`, `\$`

5. **Use regexes wisely**: `^` for beginning, `.*` for variable parts

6. **Test errors**: Verify error handling, not just success cases

7. **Document expectations**: Add comments explaining what each test checks

8. **Keep tests simple**: Use `agents_test` for most tests, custom expect only when necessary

9. **Run tests often**: After every significant change

10. **Update tests with code**: When adding features, add tests

## Summary

The AGENTS test suite uses **DejaGnu/Expect/Tcl** to automate testing of the interactive system:

**Structure**:
- **4 test suites**: format, ioports, types, old_test
- **~50 tests**: Focused on built-in predicates
- **Utilities**: agents_test simplifies writing tests
- **Logging**: agents.sum and agents.log track results

**Coverage**:
- **Good**: format/2 predicate (35 tests)
- **Basic**: I/O ports (10 tests)
- **Minimal**: Types, term operations, arithmetic
- **Missing**: Concurrency, guards, constraints, backtracking, GC, debugger

**Usage**:
```sh
make check                              # Run all tests
runtest --tool agents AGENTS=../agents  # From tests/ directory
runtest --tool agents AGENTS=../agents format.exp  # Specific suite
```

**Extending**:
- Create new .exp file in tests/agents.tests/
- Use `load_lib agents_test.exp`
- Call `agents_test name goal expected_output`
- Run with runtest

**Recommendations**:
1. **Expand coverage**: Add tests for concurrency, guards, constraints
2. **Modernize**: Replace tcov with gcov for coverage
3. **Automate**: Set up continuous integration
4. **Integrate demos**: Add demo programs to automated test suite
5. **Document**: Add comments to test suites explaining what they check

The test suite provides a **solid foundation** for regression testing and validation, but could be significantly expanded to cover the full range of AGENTS features. The DejaGnu framework makes it easy to add new tests following the established patterns.

The next chapter covers performance characteristics and benchmarking methodology.

---

**Chapter 23 complete** (~35 pages)
