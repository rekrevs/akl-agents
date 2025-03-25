# NOBIGNUM Occurrences in Emulator Source Code

This document lists all occurrences of the `NOBIGNUM` macro in the emulator source files.

## bignum.c

```c
#ifndef NOBIGNUM
// GMP-based implementation of bignum operations
// ...
#endif /* NOBIGNUM */
```

The `NOBIGNUM` macro is used to conditionally compile the GMP-based implementation of bignum operations in `bignum.c`. When `NOBIGNUM` is defined, the GMP implementation is skipped and dummy implementations are used instead.

## rational.c

```c
#if !defined(NOBIGNUM) && defined(RATNUM)
// Rational number implementation that depends on GMP
// ...
#endif /* RATNUM && !NOBIGNUM */
```

The `NOBIGNUM` macro is used in conjunction with the `RATNUM` macro to conditionally compile the rational number implementation in `rational.c`. This implementation depends on GMP and is only compiled when `RATNUM` is defined and `NOBIGNUM` is not defined.

## Configure Script

In the project's `configure` script, the `-without-gmp` flag sets the following variables:

```bash
CFLAGS_GMP=""
LDFLAGS_GMP=""
AGENTSFLAGS_GMP=-DNOBIGNUM
MAKE_GMP=""
INSTALL_GMP=""
```

When the `-without-gmp` flag is used, the `NOBIGNUM` macro is defined via the `AGENTSFLAGS_GMP` variable, and all GMP-related compiler and linker flags are removed.
