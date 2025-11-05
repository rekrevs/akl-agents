# Chapter 12: The Bug Discovery

*Or: How Porting 30-Year-Old Code Can Improve It*

---

## The Setup

It was November 5th, 2025. The x86-64 port of AGENTS was tantalizingly close. All 46 source files compiled successfully. The binary linked. When I ran it... it started.

But it didn't boot.

```bash
$ ./agents -b ../environment/boot.pam -b ../oldcompiler/comp.pam -b ../version.pam
{DOMAIN ERROR: atom_char(Overflow,~w (~w): ~w~n~w~n~w~n,_0) - arg 1: expected integer in 0..20, found Overflow}
{Configuration failed}
```

This was frustrating. The VM was clearly running - no crashes, no segfaults. The error message was coherent, formatted properly. So why "Overflow"?

---

## The Investigation Begins

The error came from `atom_char()`, a built-in function that extracts characters from atoms (symbols). It expected an integer index between 0 and the atom's length. Instead, it got the atom `Overflow`.

But where did `Overflow` come from?

### Following the Trail

A grep through the codebase revealed something interesting:

```c
// emulator/bignum.c (NOBIGNUM section)
int bignum_print(FILE *file, Bignum bn) {
    return fprintf(file, "Overflow");
}

void bignum_itoa(Bignum bn, int base, char *buf) {
    sprintf(buf, "Overflow");
}
```

Ah! When built without GMP (using `-DNOBIGNUM`), bignums are stubs. They always print as "Overflow" because there's no actual arbitrary-precision arithmetic implementation.

So the question became: **Why are bignums being created during boot?**

---

## The First Hypothesis

My initial thought: "Maybe the boot code does some computation that needs large integers?"

But that seemed unlikely. The boot sequence should just be loading bytecode and initializing the system. Why would it need numbers larger than 2^57 (the limit for "small" integers on a 64-bit system)?

### Adding Debug Output

I instrumented the bignum code:

```c
Bignum new_bignum(old) {
    fprintf(stderr, "[DEBUG: new_bignum called]\n");
    // ... rest of function
}
```

Result:

```bash
$ ./agents -b boot.pam -b comp.pam -b version.pam
[DEBUG: new_bignum called]
[DEBUG: new_bignum called]
[DEBUG: new_bignum called]
... (50+ times!)
{DOMAIN ERROR: atom_char(Overflow,...
```

**FIFTY bignums created during boot!**

This wasn't occasional. Something fundamental was wrong.

---

## The Breakthrough Moment

I stared at the code that decides whether a number needs to be a bignum:

```c
#define MaxSmallNum  (1UL << (SMALLNUMBITS-1))
#define IntIsSmall(I)  ((I) >= -MaxSmallNum && (I) < MaxSmallNum)

#define MakeIntegerTerm(X,value) \
{\
   register sword NNN = (value);\
   if (IntIsSmall(NNN)) {\
     (X) = MakeSmallNum(NNN);  // Use small number\
   } else {\
     MakeBignum(ZZZ,NNN);       // Use bignum\
     (X) = TagBig(ZZZ);\
   }\
}
```

Wait. Look at that `1UL`. That's an **unsigned long** literal.

Let me trace through what happens with `SMALLNUMBITS = 58` (derived from `TADBITS = 64`):

```c
MaxSmallNum = (1UL << 57)  // = 2^57, as unsigned
```

So far so good. But then:

```c
-MaxSmallNum  // What is this?
```

Here's the problem: **You can't negate an unsigned number and get what you expect!**

```c
MaxSmallNum = 144115188075855872   // 2^57, unsigned
-MaxSmallNum = ???
```

In C, negating an unsigned value doesn't give you a negative number. It wraps around according to two's complement arithmetic. The result is a huge positive number, not -2^57!

### The Smoking Gun

This means `IntIsSmall(I)` becomes:

```c
((I) >= huge_positive_value && (I) < MaxSmallNum)
```

**Every negative number fails this check!**

- `IntIsSmall(-1)` → FALSE
- `IntIsSmall(-100)` → FALSE
- `IntIsSmall(-1000)` → FALSE

The system was creating bignums for EVERY SINGLE NEGATIVE NUMBER, no matter how small!

---

## The "Aha!" Moment

During boot, AGENTS creates:
- Timestamps (often negative relative values)
- Counters (can be negative)
- Hash values (signed integers)
- Index offsets (negative indices)

Every one became an unnecessary bignum. And with `-DNOBIGNUM`, those bignums were useless stubs that printed as "Overflow".

When the boot sequence tried to format these values in error messages, it got "Overflow" strings instead of numbers. The `atom_char()` function received the atom `Overflow` when it expected an integer index. Boot failed.

---

## The Fix

The solution was simple - use a **signed** literal:

```c
// WRONG:
#define MaxSmallNum (1UL << (SMALLNUMBITS-1))  // unsigned

// CORRECT:
#define MaxSmallNum (1L << (SMALLNUMBITS-1))   // signed
```

With `1L` (signed long):
- `MaxSmallNum = 2^57` (signed)
- `-MaxSmallNum = -2^57` (properly negative!)
- `IntIsSmall(-1000)` → TRUE

### The Test

```bash
$ make clean && make agents
... (recompiling) ...
$ ./agents -b ../environment/boot.pam -b ../oldcompiler/comp.pam -b ../version.pam
AGENTS 0.9 (threaded code): Wed Aug 14 14:46:52 MET DST 2002
(C) 1990-1994 Swedish Institute of Computer Science
All rights reserved.
| ?-
```

**IT BOOTED!**

Not just booted - full interactive REPL:

```bash
| ?- X is 10 * 10.
X = 100 ?
```

Arithmetic worked. Unification worked. The entire system was operational - **without GMP!**

---

## Why Did This Bug Exist For 30 Years?

### It Was Masked

On the original SPARC and Alpha ports, AGENTS was always built with GMP. When bignums are real (not stubs), they:
- Store actual values
- Print correctly
- Behave like numbers

The bug caused **unnecessary bignum allocations** (wasteful), but didn't cause **failures** (the bignums still worked).

### It Affected All 64-Bit Platforms

This wasn't an x86-64 bug. The Alpha port had the same issue! But nobody noticed because:

1. GMP was always present
2. Creating extra bignums was just a performance cost, not a failure
3. The system worked, so the bug was invisible

### Testing With NOBIGNUM Exposed It

When I built without GMP (to create a minimal test binary), the bignum stubs revealed the problem immediately. The "Overflow" strings made the unnecessary allocations obvious.

**This is a case where a "temporary" testing configuration exposed a fundamental bug!**

---

## The Implications

### For The Codebase

This one-character fix (`1UL` → `1L`) improved:
- **Resource usage**: No unnecessary bignum allocations
- **Performance**: Faster integer operations (stay in small number range)
- **Portability**: NOBIGNUM configuration actually works now
- **Correctness**: IntIsSmall() functions as originally intended

### For All Platforms

The fix benefits:
- Alpha 64-bit (reduces bignum overhead)
- x86-64 (enables GMP-free operation)
- Any future 64-bit port
- Even the 32-bit SPARC port (though effect is smaller)

### For Software Archaeology

This discovery proves that **deep investigation pays off**. Instead of:
- Assuming NOBIGNUM was broken
- Working around it with GMP
- Settling for "it compiles"

We found the root cause and **improved 30-year-old code**.

---

## Lessons Learned

### 1. Signedness Matters

When doing range checks with negation:
```c
if ((x) >= -MaxValue && (x) < MaxValue)
```

`MaxValue` **must be signed**. Otherwise `-MaxValue` doesn't do what you expect.

### 2. Test Edge Cases

Building with NOBIGNUM was supposed to be a quick test. Instead, it revealed a fundamental issue. Testing "unusual" configurations can expose bugs that normal usage masks.

### 3. Trust Your Instincts

When something doesn't make sense ("Why would boot need 50 bignums?"), dig deeper. The answer is often illuminating.

### 4. Document The Journey

The discovery process itself is valuable. Future maintainers can learn from:
- The investigation methodology
- The false starts
- The moment of realization
- The validation process

### 5. Porting Can Improve Code

We didn't just port AGENTS to x86-64. We **improved** it by finding and fixing a bug that affected all platforms.

---

## The Technical Deep Dive

For those wanting complete technical details, see **Chapter 18: The Signedness Bug** which covers:

- Exact binary representation of the bug
- Why two's complement matters
- Compiler behavior analysis
- Performance impact measurements
- Complete test results

And for the historical record, see **Appendix D** for:
- Original investigation logs
- Debug session transcripts
- Email discussions (if any)
- The "aha moment" in real time

---

## Epilogue

November 5th, 2025 started as a frustrating day. The x86-64 port was "almost working" but failing mysteriously.

It ended with:
- ✅ A fully working port
- ✅ A 30-year-old bug discovered and fixed
- ✅ Improvements to all platforms
- ✅ A complete case study in software archaeology

The port succeeded beyond expectations. Not just because we made old code run on new hardware, but because we took the time to **understand** it deeply enough to improve it.

That's the real value of software archaeology: preserving the past while making it better for the future.

---

*Next: Chapter 13 - Lessons Learned*
