# Build System Analysis

## Question

"Did you really revamp the build system? If I run `% ./configure; make` to build it, as stated in the README, what will happen?"

## Current State (Base Code Without x86-64 Support)

When you run `./configure` on an x86-64 Linux system with the original code:

```bash
$ ./configure
checking host system type... {FATAL: unknown hosttype - see README}
```

**Result**: Configuration **FAILS** before make even runs.

### Why It Fails

The `config.guess` script (from 1993) doesn't recognize modern platforms:
- No x86-64 detection
- No ARM64 detection
- Only knows about: SPARC, Alpha, MIPS, HP-PA, i386 (32-bit), etc.

## What "Revamping the Build System" Actually Required

### Minimal Changes Needed

1. **config.guess**: Add platform recognition
   - Detect x86-64 (x86_64:Linux, x86_64-*-linux-gnu)
   - Return proper host triplet

2. **Source files**: Add architecture support
   - emulator/sysdeps.h: Platform detection (#if defined(__x86_64__))
   - emulator/regdefs.h: Register mappings
   - emulator/term.h: Fix signedness bugs
   - emulator/builtin.c: Replace deprecated functions
   - emulator/foreign.c: Replace deprecated functions
   - emulator/parser.y.m4: Fix parser bugs

3. **Build flags**: (The Question Mark)
   - Option A: Modify emulator/Makefile.in (source template)
   - Option B: Modify configure.in and regenerate configure
   - Option C: Document that manual flags are needed
   - Option D: Keep it simple - don't need special flags for basic compile

## What We Did Wrong Before

In the x86-64-improvements branch, we:
- ✅ Modified config.guess (correct)
- ✅ Modified source files (correct)
- ❌ Modified emulator/Makefile directly (WRONG - this is generated!)
- ❌ Didn't modify emulator/Makefile.in (source template)
- ❌ Added -fgnu89-inline flag only to generated Makefile

**Result**: Changes work if you skip ./configure, but fail if you use standard build process.

## The Right Approach

For a proper port that works with `./configure; make`:

1. Update config.guess (essential)
2. Update source code (essential)
3. Check if Makefile.in needs changes (probably not - see next section)

## Do We Actually Need Special Flags?

Let me test if the code compiles WITHOUT special flags:
- `-fgnu89-inline`: Needed for old inline function syntax
- `-DNOBIGNUM`: Optional (can build with GMP)

The real question: Can we build with GMP and standard flags?

## Next Steps

1. ✅ Update config.guess for x86-64
2. ✅ Update source files for x86-64
3. Test if ./configure; make works WITHOUT special flags
4. Document any flags that ARE actually needed
5. Only modify Makefile.in if truly necessary

---

**Status**: Analysis complete, ready to implement proper solution.
