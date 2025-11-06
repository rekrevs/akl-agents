# Testing Strategy and Validation Framework
## AGENTS v0.9 Porting Study - Phase 4, Document 7 of 10

**Version:** 1.0
**Date:** 2025-11-04
**Status:** Complete

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Current Testing Infrastructure](#current-testing-infrastructure)
3. [Test Coverage Analysis](#test-coverage-analysis)
4. [Bootstrap Verification Strategy](#bootstrap-verification-strategy)
5. [Unit Testing Strategy](#unit-testing-strategy)
6. [Integration Testing Strategy](#integration-testing-strategy)
7. [Performance Testing](#performance-testing)
8. [Regression Testing](#regression-testing)
9. [Validation Testing](#validation-testing)
10. [Modern Testing Framework Migration](#modern-testing-framework-migration)
11. [Testing Requirements by Phase](#testing-requirements-by-phase)
12. [Recommendations](#recommendations)

---

## 1. Executive Summary

**Current state:** AGENTS v0.9 has a **basic but functional** DejaGnu-based test framework from 1994.

**Key findings:**

1. **Limited test coverage:** Only 4 test suites, ~37-50 tests total
2. **Integration-focused:** Most tests are end-to-end (AGENTS executable level)
3. **No unit tests:** Individual components (GC, unification, dispatch) not tested in isolation
4. **No performance tests:** No benchmarks or performance regression detection
5. **Informal validation:** Demo programs exist but not part of automated testing

**Critical need for porting:** The incremental bootstrap approach requires **verification at each step**. Current testing is insufficient for this purpose.

**Recommended approach:**

1. **Phase 0:** Establish baseline with existing DejaGnu tests
2. **Phase 1:** Add unit tests for critical components
3. **Phase 2:** Add bootstrap verification tests (build, smoke, regression)
4. **Phase 3:** Add performance benchmarks
5. **Phase 4:** Migrate to modern testing framework (optional)

**Success criteria:**

- ✅ Every bootstrap step has pass/fail tests
- ✅ Critical components have unit tests
- ✅ Performance tracked across changes
- ✅ Regression detection automated
- ✅ Test time < 5 minutes for full suite

---

## 2. Current Testing Infrastructure

### 2.1 DejaGnu Framework

**Framework:** DejaGnu 1.x (circa 1993-1994)

**Technology stack:**
```
DejaGnu (testing framework)
  └─ Expect (interactive testing via Tcl)
      └─ Tcl (scripting language)
```

**Architecture:**

```
runtest (DejaGnu driver)
  │
  ├─ config/unix.exp          # Platform configuration, AGENTS startup
  │
  ├─ lib/agents_test.exp      # Custom test utilities
  │   ├─ agents_test_initialize
  │   └─ agents_test(name, input, expected_output)
  │
  └─ agents.tests/*.exp       # Test suites
      ├─ format.exp           # Output formatting tests
      ├─ ioports.exp          # I/O and ports tests
      ├─ types.exp            # Type system tests
      └─ old_test.exp         # Legacy comprehensive test
```

### 2.2 Test Execution

**Command:**
```bash
# From tests/ directory:
runtest --tool agents AGENTS=../agents

# Or from top-level:
make check

# Run specific suite:
runtest --tool agents AGENTS=../agents format.exp
```

**Output:**
```
Test Run By rch on Fri Jan  7 10:56:41 MET 1994
Native configuration

                === agents tests ===

Running ./agents.tests/format.exp ...
Running ./agents.tests/ioports.exp ...
Running ./agents.tests/old_test.exp ...
Running ./agents.tests/types.exp ...

                === agents Summary ===

# of expected passes       37
../agents (AGENTS 0.9)
```

**Artifacts:**
- `agents.sum` - Summary of test results
- `agents.log` - Detailed log with AGENTS output
- `agents.psum` - Previous run summary
- `agents.plog` - Previous run log

### 2.3 Test Suite Details

**File:** `tests/agents.tests/format.exp`

```tcl
load_lib agents_test.exp

agents_test_initialize

agents_test format1 \
"format('Hello, world!',\[\]).\n" \
"^Hello, world!\n"

agents_test format2 \
"format('~~',\[42\]).\n" \
"^~\n"

agents_test format3 \
"format('~d',\[42\]).\n" \
"^42\n"

# ... more tests ...
```

**Pattern:**
1. Load test utilities
2. Initialize test environment
3. For each test:
   - Send goal to AGENTS REPL
   - Match output against regex
   - Pass if matches, fail otherwise

**File:** `tests/agents.tests/old_test.exp`

```tcl
# Different style: compile and run a comprehensive test file
# Single test that runs old_test.akl
# Lumps many tests into one (not recommended style per README)
```

### 2.4 Custom Test Utilities

**File:** `tests/lib/agents_test.exp`

**Procedures:**

1. **`agents_test_initialize`**
   - Sets up test environment
   - AGENTS already running (started by DejaGnu config)

2. **`agents_test name input expected_output`**
   - `name`: Test identifier
   - `input`: Goal to send to AGENTS
   - `expected_output`: Regex to match (can match substring)
   - Sends input, expects output, reports pass/fail

**Simplicity:** Very simple utilities, easy to write tests

**Limitations:**
- Only tests REPL interaction
- No support for testing C code directly
- No support for performance measurement
- Regex matching fragile (whitespace, formatting changes)

### 2.5 Platform Configuration

**File:** `tests/config/unix.exp`

- Starts AGENTS executable
- Establishes Expect communication
- Handles cleanup on test completion

**Assumption:** UNIX-like system (hence "unix.exp")

### 2.6 Demo Programs (Informal Testing)

**Location:** `demos/`

**Programs:**
```
puzzle.akl       - N-queens puzzle
fd_cars.akl      - Finite domain constraints
life.akl         - Conway's Game of Life
ancestors.akl    - Genealogy example
orp.akl          - Or-parallelism demo
mastermind.akl   - Game
zebra.akl        - Logic puzzle
cipher.akl       - Cryptarithmetic
semigroup.akl    - Algebraic structures
... and more
```

**Status:**
- Not part of automated testing
- Manually run to verify functionality
- Good candidates for integration tests

---

## 3. Test Coverage Analysis

### 3.1 What Is Tested

**Current test coverage (estimated):**

1. **Format/output** (`format.exp`): ~10 tests
   - Basic string formatting
   - Format specifiers (~d, ~s, etc.)

2. **I/O and ports** (`ioports.exp`): ~10-15 tests
   - Port creation and communication
   - I/O operations

3. **Type system** (`types.exp`): ~5 tests
   - Type checking predicates
   - Type conversions

4. **Old comprehensive test** (`old_test.exp`): 1 test
   - Runs old_test.akl
   - Single pass/fail for many features

**Total:** ~37-50 tests (as reported in README)

### 3.2 What Is NOT Tested

**Critical gaps:**

1. **Garbage collector:**
   - No tests for GC correctness
   - No stress tests (large heaps)
   - No tests for GC edge cases (forwarding pointers, roots, etc.)

2. **Memory management:**
   - No tests for block allocation
   - No tests for heap overflow handling
   - No tests for constant space

3. **Bytecode dispatch:**
   - No tests for individual instructions
   - No tests for threaded code vs. switch dispatch
   - No tests for instruction decoding

4. **Unification:**
   - Minimal tests for core unification
   - No tests for unification edge cases
   - No tests for occurs check

5. **Tagged pointers:**
   - No tests for pointer tagging/untagging
   - No tests for tag correctness
   - No tests for 32-bit vs. 64-bit

6. **Platform dependencies:**
   - No tests for PTR_ORG handling
   - No tests for alignment
   - No tests for hard register allocation

7. **Concurrency:**
   - No tests for and-parallelism
   - No tests for or-parallelism
   - No tests for agent synchronization

8. **Finite domain constraints:**
   - No tests for FD constraint propagation
   - No tests for FD search

9. **Bignum arithmetic:**
   - No tests for GMP integration
   - No tests for small → big transitions
   - No tests for bignum overflow

10. **Build system:**
    - No tests for configure options
    - No tests for different compiler versions
    - No tests for different platforms

### 3.3 Coverage by Component

**Emulator C code coverage (estimated):**

| Component | Files | Coverage | Notes |
|-----------|-------|----------|-------|
| Main loop | engine.c | 30% | Only via integration tests |
| GC | gc.c, storage.c | 10% | Implicit via allocation |
| Unification | unify.c | 40% | Via AKL goal execution |
| Builtins | builtin.c | 50% | Via format, I/O tests |
| Parser | parser.y, parser.l | 50% | Via compile tests |
| Loader | load.c | 50% | Via load tests |
| FD constraints | fd.c, fd_akl.c | 5% | Minimal FD tests |
| BAM | bam.c | 5% | BAM not well tested |
| Foreign interface | foreign.c | 10% | Minimal foreign tests |
| **Overall** | **All** | **~30%** | **Integration only** |

**AKL code coverage:**
- Compiler: Indirectly via compile tests
- Environment: Indirectly via boot.pam loading
- Library: Minimally tested

---

## 4. Bootstrap Verification Strategy

### 4.1 Purpose

**Goal:** Ensure each step of the incremental bootstrap (see 06-BUILD-SYSTEM.md) is verifiable.

**Key principle:** Every change must have a **measurable pass/fail criterion**.

### 4.2 Baseline Establishment (Phase 0)

**Step 1: Capture baseline behavior**

```bash
# In vintage environment (QEMU Alpha or original platform):

# 1. Run all DejaGnu tests, save output
make check > baseline-dejagnu.txt 2>&1

# 2. Run all demo programs, save output
for demo in demos/*.akl; do
  ./agents $demo > baseline-$(basename $demo .akl).txt 2>&1
done

# 3. Run performance benchmarks (create if needed)
./agents demos/puzzle.akl -benchmark > baseline-puzzle-perf.txt

# 4. Compute checksums of .pam files
find . -name "*.pam" -exec sha256sum {} \; > baseline-pam-checksums.txt

# 5. Capture memory usage
/usr/bin/time -v ./agents demos/zebra.akl > baseline-memory.txt 2>&1

# 6. Capture binary characteristics
file emulator/oagents > baseline-binary.txt
size emulator/oagents >> baseline-binary.txt
nm emulator/oagents | wc -l >> baseline-binary.txt  # Symbol count
```

**Baseline artifacts:**
- Test results (DejaGnu)
- Demo outputs (deterministic programs only)
- .pam file checksums
- Performance metrics (time, memory)
- Binary characteristics

**Critical:** Store these in version control as "golden" reference.

### 4.3 Step Verification Tests

**For each modernization step:**

**Test 1: Build smoke test**
```bash
#!/bin/bash
# smoke-test.sh

set -e  # Exit on error

echo "=== Build Smoke Test ==="

# Clean build
make distclean
./configure
make all

# Check executables exist
test -f emulator/oagents || exit 1
test -x emulator/oagents || exit 1
test -f agents || exit 1
test -x agents || exit 1

# Check core files built
test -f emulator/agents.o || exit 1
test -f version.pam || exit 1

echo "✓ Build smoke test PASSED"
```

**Test 2: Basic functionality test**
```bash
#!/bin/bash
# basic-functionality.sh

set -e

echo "=== Basic Functionality Test ==="

# Test 1: AGENTS starts and exits cleanly
echo "" | timeout 5 ./agents || exit 1

# Test 2: Can evaluate simple goal
echo "format('Hello',\[\])." | timeout 5 ./agents | grep -q "Hello" || exit 1

# Test 3: Can compile simple program
cat > /tmp/test-$$.akl <<'EOF'
test :- format('OK',\[\]).
EOF
echo "compile('/tmp/test-$$')." | timeout 10 ./agents || exit 1
rm -f /tmp/test-$$.*

echo "✓ Basic functionality test PASSED"
```

**Test 3: Regression test**
```bash
#!/bin/bash
# regression-test.sh

set -e

echo "=== Regression Test ==="

# Run DejaGnu tests
make check > current-dejagnu.txt 2>&1

# Compare pass count
baseline_passes=$(grep "# of expected passes" baseline-dejagnu.txt | awk '{print $5}')
current_passes=$(grep "# of expected passes" current-dejagnu.txt | awk '{print $5}')

if [ "$baseline_passes" != "$current_passes" ]; then
  echo "✗ FAIL: Expected $baseline_passes passes, got $current_passes"
  exit 1
fi

# Check for new failures
if grep -q "# of unexpected failures" current-dejagnu.txt; then
  echo "✗ FAIL: Unexpected failures detected"
  exit 1
fi

echo "✓ Regression test PASSED ($current_passes tests)"
```

**Test 4: Demo programs test**
```bash
#!/bin/bash
# demo-test.sh

set -e

echo "=== Demo Programs Test ==="

# Test deterministic demos
for demo in puzzle ancestors zebra; do
  echo "Testing $demo..."
  timeout 30 ./agents demos/$demo.akl > current-$demo.txt 2>&1

  # Compare output (allowing minor differences)
  if ! diff -q baseline-$demo.txt current-$demo.txt > /dev/null 2>&1; then
    echo "⚠ WARNING: $demo output differs (may be acceptable)"
    # Don't fail, just warn (output may have timestamps, etc.)
  fi
done

echo "✓ Demo programs test PASSED"
```

**Test 5: Performance test**
```bash
#!/bin/bash
# performance-test.sh

echo "=== Performance Test ==="

# Benchmark puzzle
start=$(date +%s%N)
timeout 60 ./agents demos/puzzle.akl > /dev/null 2>&1
end=$(date +%s%N)
current_time=$(( ($end - $start) / 1000000 ))  # milliseconds

baseline_time=$(grep "Time:" baseline-puzzle-perf.txt | awk '{print $2}')

# Allow 20% performance variance
max_time=$(( $baseline_time * 120 / 100 ))

if [ $current_time -gt $max_time ]; then
  echo "⚠ WARNING: Performance degradation: ${current_time}ms vs ${baseline_time}ms baseline"
  # Don't fail, just warn
else
  echo "✓ Performance test PASSED: ${current_time}ms (baseline ${baseline_time}ms)"
fi
```

**Test 6: Memory safety test**
```bash
#!/bin/bash
# memory-safety.sh

echo "=== Memory Safety Test ==="

# Run with valgrind if available
if command -v valgrind > /dev/null; then
  echo "format('test',\[\])." | valgrind --leak-check=full --error-exitcode=1 ./agents 2>&1 | tee valgrind.log

  if grep -q "ERROR SUMMARY: 0 errors" valgrind.log; then
    echo "✓ Memory safety test PASSED"
  else
    echo "✗ FAIL: Memory errors detected"
    exit 1
  fi
else
  echo "⚠ SKIP: valgrind not available"
fi
```

### 4.4 Verification Test Suite (Combined)

**File:** `tests/verify-bootstrap.sh`

```bash
#!/bin/bash
# verify-bootstrap.sh
# Run all verification tests for a bootstrap step

set -e

TESTS_DIR=$(dirname "$0")
cd "$TESTS_DIR/.."

echo "======================================"
echo "Bootstrap Verification Test Suite"
echo "======================================"
echo ""

# Run all verification tests
bash tests/smoke-test.sh
bash tests/basic-functionality.sh
bash tests/regression-test.sh
bash tests/demo-test.sh
bash tests/performance-test.sh
bash tests/memory-safety.sh

echo ""
echo "======================================"
echo "✓ ALL VERIFICATION TESTS PASSED"
echo "======================================"
```

**Usage at each bootstrap step:**

```bash
# After making change (e.g., updating config.guess):
./configure && make clean && make all
bash tests/verify-bootstrap.sh

# If passes, commit and proceed
# If fails, debug and fix
```

---

## 5. Unit Testing Strategy

### 5.1 Rationale

**Problem:** Current tests are all integration tests (test full AGENTS executable).

**Issues with integration-only testing:**
1. **Slow feedback:** Must build entire system
2. **Hard to isolate bugs:** Failure could be anywhere
3. **Hard to test edge cases:** Need to construct complex AKL programs
4. **No coverage of internal behavior:** Can't test GC directly

**Solution:** Add unit tests for critical C components.

### 5.2 Testable Components

**Priority 1: Core data structures**

1. **Tagged pointers** (`term.h`, `config.c`)
   ```c
   // Test: TagPtr, PTagOf, AddressOf, IsNUM, IsLST, etc.
   void test_tagged_pointers() {
     // Test small number tagging
     Term t = MakeSmallNum(42);
     assert(IsNUM(t));
     assert(GetSmall(t) == 42);

     // Test list tagging
     Term lst = TagLst(some_list_address);
     assert(IsLST(lst));
     assert(AddressOf(lst) == some_list_address);

     // Test 32-bit vs 64-bit
     #if TADBITS == 64
     Term big = MakeSmallNum(0x7FFFFFFFLL);
     assert(IsNUM(big));
     #endif
   }
   ```

2. **Atom creation and hashing** (`config.c`, `functor.c`)
   ```c
   void test_atoms() {
     Atom a1 = make_atom("test", hash_string("test"));
     Atom a2 = make_atom("test", hash_string("test"));
     assert(a1->pname == a2->pname);  // Interned

     Atom a3 = make_atom("foo", hash_string("foo"));
     assert(strcmp(a3->pname, "foo") == 0);
   }
   ```

3. **Functor creation** (`config.c`)
   ```c
   void test_functors() {
     Atom name = make_atom("f", 42);
     Functor f = make_functor(name, 3);
     assert(f->name == name);
     assert(f->arity == 3);
     assert(f->nums == 3);
   }
   ```

**Priority 2: Memory management**

4. **Block allocation** (`storage.c`)
   ```c
   void test_block_allocation() {
     block *b = heap_block();
     assert(b != NULL);
     assert(b->flipflop == currentflip);

     // Test allocation fills blocks
     for (int i = 0; i < 100; i++) {
       Term *p = allocate_on_heap(10);
       assert(p != NULL);
     }
   }
   ```

5. **Forwarding pointers** (`gc.c`)
   ```c
   void test_forwarding() {
     Term t = TagLst(some_address);
     Term forwarded = set_forwarding_pointer(t, new_address);

     assert(is_forwarded(t));
     assert(get_forwarding_address(t) == new_address);
   }
   ```

**Priority 3: Arithmetic and bignums**

6. **Small number operations** (`builtin.c`, `config.c`)
   ```c
   void test_small_arithmetic() {
     Term a = MakeSmallNum(10);
     Term b = MakeSmallNum(20);
     Term sum = small_add(a, b);
     assert(GetSmall(sum) == 30);

     // Test overflow detection
     Term max = MakeSmallNum(MaxSmallNum - 1);
     Term overflow = small_add(max, MakeSmallNum(2));
     assert(IsBIG(overflow));  // Should promote to bignum
   }
   ```

7. **Bignum operations** (`bignum.c`)
   ```c
   void test_bignum() {
     Term big = bignum_atoi("12345678901234567890");
     assert(IsBIG(big));

     double f = get_bignum_float(big);
     assert(f > 1.0e19);

     char buf[100];
     bignum_itoa(Big(big), 10, buf);
     assert(strcmp(buf, "12345678901234567890") == 0);
   }
   ```

**Priority 4: Unification**

8. **Basic unification** (`unify.c`)
   ```c
   void test_unify() {
     Term x = MakeUVA(some_address);
     Term y = MakeSmallNum(42);

     bool result = unify(x, y);
     assert(result == TRUE);
     assert(deref(x) == y);
   }
   ```

### 5.3 Unit Test Framework

**Option 1: Minimal C framework (recommended for initial tests)**

**File:** `emulator/tests/unit_test.h`

```c
#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include <stdio.h>
#include <stdlib.h>

#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
  printf("Running %s... ", #name); \
  test_##name(); \
  printf("✓\n"); \
} while(0)

#define ASSERT(cond) do { \
  if (!(cond)) { \
    fprintf(stderr, "ASSERTION FAILED: %s at %s:%d\n", #cond, __FILE__, __LINE__); \
    exit(1); \
  } \
} while(0)

#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_NE(a, b) ASSERT((a) != (b))

#endif
```

**File:** `emulator/tests/test_tagged_pointers.c`

```c
#include "../include.h"
#include "../term.h"
#include "unit_test.h"

TEST(small_number_tagging) {
  Term t = MakeSmallNum(42);
  ASSERT(IsNUM(t));
  ASSERT_EQ(GetSmall(t), 42);
}

TEST(list_tagging) {
  uword addr = 0x12345678;
  Term lst = TagLst((cons*)addr);
  ASSERT(IsLST(lst));
  ASSERT_EQ((uword)AddressOf(lst), addr & ~PTagMask);
}

TEST(negative_numbers) {
  Term t = MakeSmallNum(-42);
  ASSERT(IsNUM(t));
  ASSERT_EQ(GetSmall(t), -42);
}

int main() {
  printf("=== Tagged Pointer Tests ===\n");
  RUN_TEST(small_number_tagging);
  RUN_TEST(list_tagging);
  RUN_TEST(negative_numbers);
  printf("All tests passed!\n");
  return 0;
}
```

**Makefile target:**

```makefile
# emulator/Makefile

test-unit: test_tagged_pointers test_atoms test_storage
	./test_tagged_pointers
	./test_atoms
	./test_storage

test_tagged_pointers: tests/test_tagged_pointers.c $(DEF)
	$(CC) $(CFLAGS) tests/test_tagged_pointers.c config.o -o test_tagged_pointers

test_atoms: tests/test_atoms.c $(DEF)
	$(CC) $(CFLAGS) tests/test_atoms.c config.o functor.o storage.o -o test_atoms

test_storage: tests/test_storage.c $(DEF)
	$(CC) $(CFLAGS) tests/test_storage.c storage.o -o test_storage
```

**Option 2: Modern C test framework (for later)**

- **Check:** http://check.sourceforge.net/
- **CUnit:** http://cunit.sourceforge.net/
- **Unity:** https://github.com/ThrowTheSwitch/Unity
- **Greatest:** https://github.com/silentbicycle/greatest

**Recommendation:** Start with minimal framework, migrate to Check or Unity later.

### 5.4 Test-Driven Modernization

**Process for each component:**

1. **Write unit test for current behavior**
2. **Verify test passes with current code**
3. **Make change (e.g., replace getwd with getcwd)**
4. **Verify unit test still passes**
5. **Commit**

**Example: Testing getwd replacement**

```c
// Before writing unit test, current code in foreign.c:
getwd(cwd);

// Unit test:
TEST(get_working_directory) {
  char cwd_before[MAXPATHLEN+1];
  char cwd_after[MAXPATHLEN+1];

  // Get CWD using old method
  getwd(cwd_before);

  // Change directory
  char *original = getcwd(NULL, 0);
  chdir("/tmp");

  // Get CWD again
  getwd(cwd_after);
  ASSERT(strcmp(cwd_after, "/tmp") == 0);

  // Restore
  chdir(original);
  free(original);
}

// After replacement with getcwd:
getcwd(cwd, sizeof(cwd));

// Run unit test again, should still pass
```

---

## 6. Integration Testing Strategy

### 6.1 Current Integration Tests

**DejaGnu tests are integration tests:**
- Test full AGENTS executable
- Test from AKL language level
- Test end-to-end workflows

**Keep these,** but enhance:

### 6.2 Enhanced Integration Tests

**Add tests for critical workflows:**

1. **Compilation workflow**
   ```tcl
   agents_test compile_workflow1 \
   "compilef('demos/puzzle').\n" \
   "^.*yes.*\n"

   # Verify .pam file created
   # Verify .pam file loadable
   ```

2. **Loading workflow**
   ```tcl
   # First compile
   agents_test_exec "compilef('demos/puzzle').\n"

   # Then load
   agents_test load_workflow1 \
   "load('demos/puzzle').\n" \
   "^.*yes.*\n"
   ```

3. **Foreign interface workflow**
   ```tcl
   # Test foreign function registration
   # Test calling foreign functions
   # Test foreign data marshaling
   ```

4. **Garbage collection workflow**
   ```tcl
   # Test program that allocates heavily
   # Verify GC occurs (via metering?)
   # Verify no crashes
   ```

5. **Concurrency workflow**
   ```tcl
   # Test and-parallelism
   # Test or-parallelism
   # Test port communication
   ```

### 6.3 Demo Programs as Integration Tests

**Automate demo testing:**

**File:** `tests/demo-integration.sh`

```bash
#!/bin/bash
# Run all demo programs as integration tests

set -e

DEMOS="
  puzzle
  ancestors
  zebra
  cipher
  life
"

echo "=== Demo Integration Tests ==="

for demo in $DEMOS; do
  echo -n "Testing $demo... "

  # Run with timeout
  timeout 60 ./agents demos/$demo.akl > /tmp/demo-$demo.txt 2>&1

  # Check for success indicators (demo-specific)
  case $demo in
    puzzle)
      grep -q "Solution:" /tmp/demo-$demo.txt || exit 1
      ;;
    ancestors)
      grep -q "ancestor" /tmp/demo-$demo.txt || exit 1
      ;;
    zebra)
      grep -q "Norwegian" /tmp/demo-$demo.txt || exit 1
      ;;
  esac

  echo "✓"
done

echo "All demo integration tests passed!"
```

**Add to Makefile:**

```makefile
check-demos:
	bash tests/demo-integration.sh
```

### 6.4 System-Level Integration Tests

**Test complete workflows:**

1. **Build from scratch**
   ```bash
   make distclean
   ./configure
   make all
   make check
   ```

2. **Install and use**
   ```bash
   make install prefix=/tmp/agents-test
   /tmp/agents-test/bin/agents -c "format('test',\[\])."
   ```

3. **Cross-component integration**
   - Compile AKL → load .pam → execute → GC → port I/O

---

## 7. Performance Testing

### 7.1 Current State

**No performance tests currently.**

**Need:**
- Benchmarks for key operations
- Performance regression detection
- Comparison across architectures

### 7.2 Benchmark Suite

**Create benchmark programs:**

**Benchmark 1: Arithmetic**
```akl
% File: benchmarks/arithmetic.akl
% Test: Small number arithmetic performance

benchmark_arithmetic(N) :-
  arithmetic_loop(N, 0).

arithmetic_loop(0, Sum) :-
  format('Sum: ~d~n', [Sum]).
arithmetic_loop(N, Sum) :-
  N > 0,
  N1 is N - 1,
  Sum1 is Sum + N,
  arithmetic_loop(N1, Sum1).

% Run: benchmark_arithmetic(1000000).
```

**Benchmark 2: Unification**
```akl
% Test: Unification performance

benchmark_unify(N) :-
  unify_loop(N).

unify_loop(0).
unify_loop(N) :-
  N > 0,
  X = f(a, b, c, d, e),
  X = f(A, B, C, D, E),
  N1 is N - 1,
  unify_loop(N1).
```

**Benchmark 3: List operations**
```akl
% Test: List construction and traversal

benchmark_lists(N) :-
  make_list(N, L),
  sum_list(L, Sum),
  format('Sum: ~d~n', [Sum]).

make_list(0, []).
make_list(N, [N|Rest]) :-
  N > 0,
  N1 is N - 1,
  make_list(N1, Rest).

sum_list([], 0).
sum_list([H|T], Sum) :-
  sum_list(T, RestSum),
  Sum is H + RestSum.
```

**Benchmark 4: Garbage collection stress**
```akl
% Test: GC performance under stress

benchmark_gc(N) :-
  gc_stress_loop(N).

gc_stress_loop(0).
gc_stress_loop(N) :-
  N > 0,
  % Allocate large structure
  make_list(1000, _),
  N1 is N - 1,
  gc_stress_loop(N1).
```

### 7.3 Performance Measurement Harness

**File:** `tests/benchmark.sh`

```bash
#!/bin/bash
# benchmark.sh - Run performance benchmarks

BENCHMARKS="arithmetic unify lists gc"

echo "=== AGENTS Performance Benchmarks ==="
echo ""

for bench in $BENCHMARKS; do
  echo "Running $bench benchmark..."

  # Run with time
  /usr/bin/time -f "Time: %E  Memory: %M KB" \
    ./agents benchmarks/$bench.akl 2>&1 | tee benchmarks/$bench.result

  echo ""
done

# Generate summary
echo "=== Summary ==="
for bench in $BENCHMARKS; do
  time=$(grep "Time:" benchmarks/$bench.result | awk '{print $2}')
  mem=$(grep "Memory:" benchmarks/$bench.result | awk '{print $4}')
  echo "$bench: $time, $mem KB"
done
```

### 7.4 Performance Regression Detection

**Compare across changes:**

```bash
#!/bin/bash
# perf-compare.sh

# Baseline results (from baseline-perf/)
# Current results (from benchmarks/)

echo "=== Performance Comparison ==="

for bench in arithmetic unify lists gc; do
  baseline=$(grep "Time:" baseline-perf/$bench.result | awk '{print $2}')
  current=$(grep "Time:" benchmarks/$bench.result | awk '{print $2}')

  # Convert to milliseconds (assumes format like "0:00.50")
  baseline_ms=$(echo $baseline | awk -F: '{split($2,a,"."); print a[1]*1000 + a[2]*10}')
  current_ms=$(echo $current | awk -F: '{split($2,a,"."); print a[1]*1000 + a[2]*10}')

  percent=$(( 100 * (current_ms - baseline_ms) / baseline_ms ))

  if [ $percent -gt 10 ]; then
    echo "⚠ $bench: REGRESSION ${percent}% slower ($current vs $baseline)"
  elif [ $percent -lt -10 ]; then
    echo "✓ $bench: IMPROVEMENT ${percent}% faster ($current vs $baseline)"
  else
    echo "✓ $bench: STABLE ${percent}% change ($current vs $baseline)"
  fi
done
```

### 7.5 Architecture Comparison

**Compare x86-64 vs ARM64 vs Alpha:**

```bash
# Run on each architecture
# Collect results
# Compare

# File: perf-matrix.txt
#
# Benchmark    Alpha(baseline)  x86-64    ARM64    x86-64/Alpha  ARM64/Alpha
# arithmetic   100ms            95ms      105ms    0.95x         1.05x
# unify        200ms            180ms     210ms    0.90x         1.05x
# lists        150ms            140ms     160ms    0.93x         1.07x
# gc           500ms            450ms     520ms    0.90x         1.04x
```

---

## 8. Regression Testing

### 8.1 Purpose

**Detect unintended changes** in behavior during modernization.

### 8.2 Regression Test Strategy

**Test at multiple levels:**

1. **DejaGnu tests** (existing)
   - Must continue to pass
   - Same number of passes

2. **Demo outputs** (deterministic ones)
   - Puzzle solutions
   - Zebra puzzle answers
   - Cipher solutions

3. **.pam file checksums**
   - Compiler output should be identical
   - Or document why it changed

4. **Performance benchmarks**
   - No more than 10-20% regression

### 8.3 Regression Test Workflow

**Before major change:**

```bash
# Capture current state
make check > regression-baseline.txt
bash tests/benchmark.sh
cp -r benchmarks regression-baseline-perf
```

**After change:**

```bash
# Test new state
make check > regression-current.txt
bash tests/benchmark.sh

# Compare
diff regression-baseline.txt regression-current.txt
bash tests/perf-compare.sh
```

**Accept or reject:**

- ✅ Accept if: All tests pass, performance within range
- ✅ Accept if: Intentional change, documented
- ❌ Reject if: Unexpected failures or regressions

---

## 9. Validation Testing

### 9.1 Purpose

**Validate correctness** of core algorithms and data structures.

### 9.2 Validation Tests

**Test 1: GC correctness**

```c
// Validate GC preserves reachable objects
void test_gc_preserves_reachable() {
  // Create reachable structure
  Term root = make_complex_structure();

  // Force GC
  force_garbage_collection();

  // Verify structure still valid
  validate_structure(root);
}

// Validate GC collects unreachable objects
void test_gc_collects_unreachable() {
  size_t before = heap_used();

  // Allocate temporary objects
  for (int i = 0; i < 1000; i++) {
    Term temp = make_complex_structure();
    // Don't save reference
  }

  // Force GC
  force_garbage_collection();

  size_t after = heap_used();

  // Memory should be reclaimed
  assert(after < before + threshold);
}
```

**Test 2: Unification soundness**

```akl
% Test unification axioms

test_unification :-
  % Reflexivity: X = X
  X = X,

  % Symmetry: if X = Y then Y = X
  (X = Y -> Y = X),

  % Transitivity: if X = Y and Y = Z then X = Z
  (X = Y, Y = Z -> X = Z),

  % Occurs check
  \+ X = f(X).
```

**Test 3: Arithmetic correctness**

```akl
test_arithmetic :-
  % Integer arithmetic
  42 is 40 + 2,
  10 is 20 - 10,
  20 is 4 * 5,
  5 is 20 / 4,
  2 is 20 mod 9,

  % Floating point
  4.0 is 2.0 * 2.0,

  % Mixed
  42.0 is 40 + 2.0,

  % Overflow to bignum
  Big is 999999999999999999 + 1,
  Big > 999999999999999999.
```

**Test 4: Pointer tagging correctness**

```c
void test_pointer_tagging_correctness() {
  // Test all tag values
  for (tval tag = REF; tag <= GEN; tag++) {
    uword addr = 0x12345678;
    Term t = TagPtr(tag, (void*)addr);

    assert(PTagOf(t) == tag);
    assert((uword)AddressOf(t) == (addr & ~PTagMask));
  }

  // Test 64-bit addresses (if TADBITS == 64)
  #if TADBITS == 64
  uword addr64 = 0x123456789ABCDEF0UL;
  Term t64 = TagPtr(STR, (void*)addr64);
  assert((uword)AddressOf(t64) == (addr64 & ~PTagMask));
  #endif
}
```

### 9.3 Property-Based Testing (Advanced)

**Consider using property-based testing for validation:**

- **QuickCheck-style testing** for AKL code
- **Generate random test cases**
- **Verify properties hold**

**Example properties:**

```akl
% Property: append is associative
property_append_associative :-
  % For all lists A, B, C:
  append(A, B, AB),
  append(AB, C, ABC1),
  append(B, C, BC),
  append(A, BC, ABC2),
  ABC1 = ABC2.  % Should always be true
```

---

## 10. Modern Testing Framework Migration

### 10.1 Limitations of DejaGnu

**Issues:**

1. **Old technology:** Based on Expect/Tcl (1990s)
2. **Limited expressiveness:** Regex matching fragile
3. **Poor error messages:** Hard to debug failures
4. **No test discovery:** Manual test registration
5. **No parallel execution:** Slow for large suites
6. **No modern features:** No fixtures, no parameterized tests, no mocking

### 10.2 Modern Alternatives

**Option 1: Python pytest (recommended)**

**Advantages:**
- ✅ Modern, actively maintained
- ✅ Excellent error messages
- ✅ Parallel execution (pytest-xdist)
- ✅ Easy to write tests
- ✅ Rich ecosystem (fixtures, plugins)

**Example test:**

```python
# tests/test_agents.py

import subprocess
import pytest

def agents(goal):
    """Helper: Run AGENTS with goal, return output"""
    proc = subprocess.run(
        ['./agents'],
        input=goal + '\n',
        capture_output=True,
        text=True,
        timeout=5
    )
    return proc.stdout

def test_format_hello():
    output = agents("format('Hello, world!',\[\]).")
    assert "Hello, world!" in output

def test_arithmetic():
    output = agents("X is 40 + 2, format('~d',\[X\]).")
    assert "42" in output

@pytest.mark.parametrize("n,expected", [
    (0, "0"),
    (42, "42"),
    (-10, "-10"),
])
def test_number_formatting(n, expected):
    output = agents(f"format('~d',\[{n}\]).")
    assert expected in output

# Run: pytest tests/test_agents.py
```

**Option 2: Bash/shell scripts (simple)**

**Advantages:**
- ✅ No dependencies
- ✅ Easy to understand
- ✅ Portable

**Example:**

```bash
#!/bin/bash
# tests/test-agents.sh

test_count=0
pass_count=0
fail_count=0

test_case() {
  local name=$1
  local goal=$2
  local expected=$3

  test_count=$((test_count + 1))

  output=$(echo "$goal" | timeout 5 ./agents 2>&1)

  if echo "$output" | grep -q "$expected"; then
    echo "✓ $name"
    pass_count=$((pass_count + 1))
  else
    echo "✗ $name"
    echo "  Expected: $expected"
    echo "  Got: $output"
    fail_count=$((fail_count + 1))
  fi
}

# Tests
test_case "format hello" "format('Hello',\[\])." "Hello"
test_case "arithmetic" "X is 40 + 2, format('~d',\[X\])." "42"

# Summary
echo ""
echo "$pass_count / $test_count tests passed"
exit $fail_count
```

**Option 3: TAP (Test Anything Protocol)**

**Advantages:**
- ✅ Language-agnostic
- ✅ Many consumers (prove, jenkins, etc.)
- ✅ Simple format

**Example:**

```bash
#!/bin/bash
# tests/test-tap.sh

echo "1..3"  # Test plan: 3 tests

# Test 1
if echo "format('Hello',\[\])." | ./agents | grep -q "Hello"; then
  echo "ok 1 - format hello"
else
  echo "not ok 1 - format hello"
fi

# Test 2
if echo "X is 40 + 2." | ./agents | grep -q "42"; then
  echo "ok 2 - arithmetic"
else
  echo "not ok 2 - arithmetic"
fi

# Test 3
if echo "X = f(a,b,c)." | ./agents | grep -q "yes"; then
  echo "ok 3 - unification"
else
  echo "not ok 3 - unification"
fi

# Run: prove tests/test-tap.sh
```

### 10.3 Migration Strategy

**Phase 1: Keep DejaGnu, add unit tests**
- Don't disrupt existing tests
- Add C unit tests (minimal framework)

**Phase 2: Add Python pytest for new integration tests**
- Easier to write than DejaGnu
- Can coexist with DejaGnu

**Phase 3: Gradually migrate DejaGnu to pytest**
- One test suite at a time
- Keep both running until migration complete

**Phase 4: Remove DejaGnu (optional)**
- Only if pytest covers all cases
- Document why keeping DejaGnu might be valuable (historical)

---

## 11. Testing Requirements by Phase

### 11.1 Phase 0: Baseline Establishment

**Testing:**
- ✅ Run all DejaGnu tests, save output
- ✅ Run all demos, save output (deterministic ones)
- ✅ Create performance baseline
- ✅ Save .pam checksums
- ✅ Document baseline environment

**Deliverables:**
- `baseline-dejagnu.txt`
- `baseline-demo-*.txt`
- `baseline-perf/`
- `baseline-pam-checksums.txt`
- `BASELINE.md` (documentation)

### 11.2 Phase 1: Modernization in Vintage Environment

**Testing at each step:**
- ✅ Build smoke test
- ✅ Basic functionality test
- ✅ Regression test (DejaGnu)
- ✅ Demo test
- ✅ Performance test

**New tests to add:**
- ✅ Unit tests for deprecated function replacements
  - `test_getcwd` (replacing getwd)
  - `test_snprintf` (replacing gcvt)
- ✅ Build system tests
  - Configure with different options
  - Test on different gcc versions

### 11.3 Phase 2: Port to x86-64

**Testing:**
- ✅ All Phase 1 tests (must pass)
- ✅ New unit tests:
  - `test_64bit_pointers`
  - `test_hard_registers_x64`
- ✅ Memory safety tests:
  - valgrind
  - AddressSanitizer
  - UndefinedBehaviorSanitizer
- ✅ Alignment tests
- ✅ Performance comparison vs. Alpha

**Critical tests:**
- ✅ Large heap test (>4GB addresses)
- ✅ GC stress test
- ✅ All demos run correctly

### 11.4 Phase 3: Port to ARM64

**Testing:**
- ✅ All previous tests
- ✅ Alignment tests (strict):
  - `test_arm64_alignment`
- ✅ Hard register tests:
  - `test_hard_registers_arm64`
- ✅ Cross-platform consistency:
  - Same .pam files on x86-64 and ARM64
  - Same output for demos

### 11.5 Phase 4: Finalization

**Testing:**
- ✅ Full test suite on all platforms
- ✅ CI/CD pipeline automated tests
- ✅ Performance matrix documented
- ✅ All unit tests passing
- ✅ All integration tests passing
- ✅ No memory leaks (valgrind)
- ✅ No undefined behavior (sanitizers)

---

## 12. Recommendations

### 12.1 Immediate Actions (Week 1)

1. **Establish baseline:**
   ```bash
   # In vintage environment or current system:
   make check > baseline-dejagnu.txt
   # Save this file in version control
   ```

2. **Create bootstrap verification scripts:**
   - `smoke-test.sh`
   - `basic-functionality.sh`
   - `regression-test.sh`

3. **Document baseline environment:**
   - Platform (Alpha, SPARC, x86-64)
   - OS version
   - GCC version
   - All test results

### 12.2 Short-Term Goals (Month 1)

1. **Add unit tests for 5-10 critical components:**
   - Tagged pointers
   - Atoms and functors
   - Small arithmetic
   - Basic unification
   - Block allocation

2. **Create performance benchmarks:**
   - Arithmetic benchmark
   - Unification benchmark
   - List operations benchmark
   - GC stress benchmark

3. **Automate demo testing:**
   - `demo-integration.sh`
   - Add to `make check-demos`

### 12.3 Medium-Term Goals (Months 2-3)

1. **Expand unit test coverage to 20-30 tests**

2. **Add memory safety testing:**
   - valgrind integration
   - AddressSanitizer builds

3. **Create verification tests for each bootstrap step:**
   - One test suite per phase
   - Automated in CI/CD

4. **Start pytest migration:**
   - Write new integration tests in Python
   - Keep DejaGnu running

### 12.4 Long-Term Goals (Months 4-6)

1. **Achieve 50%+ unit test coverage**

2. **Complete pytest migration**

3. **Full CI/CD pipeline:**
   - Build on x86-64, ARM64
   - Run all tests automatically
   - Performance tracking
   - Code coverage reporting

4. **Validation testing:**
   - Property-based tests
   - Formal verification of critical algorithms

### 12.5 Success Criteria

**Phase 0 success:**
- ✅ Baseline established and documented
- ✅ All existing tests passing
- ✅ Reproducible environment

**Phase 1-2 success:**
- ✅ Every bootstrap step verified
- ✅ No test regressions
- ✅ Performance within 10% of baseline

**Phase 3 success:**
- ✅ 20+ unit tests
- ✅ All integration tests automated
- ✅ Performance benchmarks in place
- ✅ Memory safety validated

**Final success:**
- ✅ Comprehensive test suite (100+ tests)
- ✅ All tests passing on x86-64 and ARM64
- ✅ No memory leaks or undefined behavior
- ✅ Performance documented and acceptable
- ✅ CI/CD pipeline operational

---

## Conclusion

A **robust testing strategy** is critical for successful incremental porting. The current DejaGnu tests provide a foundation, but significant enhancements are needed:

1. **Unit tests** for isolated component verification
2. **Bootstrap verification tests** for each modernization step
3. **Performance benchmarks** for regression detection
4. **Automated demo testing** for integration validation
5. **Memory safety testing** for correctness assurance

**Key insight:** Testing is not just about finding bugs—it's about **enabling confident forward progress** in an incremental bootstrap strategy. Every step must be verifiable, or we risk compounding errors across multiple changes.

**Next steps:**

1. Create document 08-RISK-ASSESSMENT.md (detailed risk analysis)
2. Create document 09-IMPLEMENTATION-ROADMAP.md (detailed step-by-step plan)
3. Begin Phase 0 (baseline establishment)
4. Implement bootstrap verification tests

---

**Document Status:** Complete
**Last Updated:** 2025-11-04
**Next Document:** 08-RISK-ASSESSMENT.md
