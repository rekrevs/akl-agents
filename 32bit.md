# 32-bit Architecture Dependencies in the Emulator

This document lists all dependencies on 32-bit architecture implementation found in the emulator codebase.

## term.h

- **Line 8-19**: The tagged pointer scheme is explicitly designed for 32-bit words:
  ```
  /* The layout looks like this for machines with 32-bit words:
   *
   * Reference:				0:::::::::::::::::::::::::::::00
   * Unbound unconstrained variable:	0:::::::::::::::::::::::::::::01
   * Unbound constrained variable:	0:::::::::::::::::::::::::::::11
   * List cell:				0:::::::::::::::::::::::::::0010
   * Structure:				0:::::::::::::::::::::::::::1010
   * Generic, float, or bignum:		0:::::::::::::::::::::::::::1110
   * Small integer:			0::::::::::::::::::::::::::00110
   * Atom:				0::::::::::::::::::::::::::10110
   */
  ```

- **Line 76-81**: Explicit definition of bit size with default of 32 bits:
  ```
  /* TADBITS is the number of bits in a tagged address word.
   * You must change this and the bitmasks below, if you have a different
   * word length than 32 bits (like on the DEC Alpha).
   */
  #ifndef TADBITS
  #define TADBITS 32
  #endif
  ```

- **Line 89**: Word alignment based on 32-bit assumption:
  ```
  #define WORDALIGNMENT	(TADBITS/8)
  ```

- **Line 95**: Small number bits calculation assumes 32-bit word size:
  ```
  #define SMALLNUMBITS	(TADBITS-6)
  ```

- **Line 107**: Mark bit mask assumes 32-bit word:
  ```
  #define MarkBitMask	((uword)0x1 << (TADBITS-1))
  ```

## sysdeps.h

- **Line 1-3**: Special handling for 64-bit Alpha architecture:
  ```
  #ifdef __alpha__
  #define TADBITS 64
  #endif
  ```

- **Line 19-22**: HP-PA architecture has a specific pointer origin value:
  ```
  # ifdef hppa
  #  define PTR_ORG	((uword)0x40000000)
  # endif
  ```

- **Line 38-41**: AIX has a specific pointer origin value:
  ```
  #ifdef _AIX
  #define unix
  #define PTR_ORG		((uword)0x20000000)
  #endif
  ```

## bignum.c

- **Line 237-238**: Sign bit handling specifically for 32-bit architecture:
  ```
  #define SIGN_BIT	(1+((unsigned long)LONG_MAX))	/* 0x8000000    */
  #define F_SIGN_BIT	(1.0+((double)LONG_MAX))	/* 2147483648.0 */
  ```

- **Lines 244-285**: The `get_bignum_float` function assumes 32-bit integers when converting bignums to floating point.

- **Lines 289-314**: The `bignum_from_float` function assumes 32-bit integers when converting floating point to bignums.

## storage.h

- **Line 4**: Word alignment function assumes 32-bit word size:
  ```
  #define WordAlign(x) (((x) + (WORDALIGNMENT-1)) & ~((uword) (WORDALIGNMENT-1)))
  ```

- **Line 78-91**: Special handling for float alignment assumes 32-bit boundaries:
  ```
  #define NewFloat(ret)\
  {\
      if((uword)heapcurrent % sizeof(double) != 0) {\
         heapcurrent += sizeof(double) - (uword)heapcurrent % sizeof(double);\
      }\
      (ret) = (Fp) heapcurrent; \
      heapcurrent += FltSize; \
      if(heapcurrent >= heapend) {\
          SaveHeapRegisters(); \
          reinit_heap((ret),SMALL_SIZE); \
          RestoreHeapRegisters(); \
          (ret) = (Fp) heapcurrent; \
          heapcurrent += FltSize; \
      }\
  }
  ```

## engine.c

- **Lines 40-46**: Register allocation for the engine assumes 32-bit architecture:
  ```
  register code		*pc REGISTER3;	/* The code pointer */
  register opcode	op REGISTER4;	/* The next opcode */
  register Term 	*areg REGISTER5; 	/* The argument registers */
  register Term 	*yreg REGISTER6 = NULL;	/* Current environment */
  register int		write_mode REGISTER1 = 0;
  register andbox 	*andb REGISTER2;	/* The current and box */
  ```

## hash.h

- **Lines 126-134**: Hash table size calculations assume 32-bit words:
  ```
  #define NewHashTable(Table,Size,ProcAllocate) \
  { \
    unsigned long __real_size = 0x4; \
  \
    if (Size & ~((unsigned long)0xff)) /* Always start at 256 or less */ \
      __real_size = 0x100; \
  \
    while (__real_size < Size)	/* Find power of 2 that fit */ \
      __real_size = (__real_size << 1); \
  \
    AllocateHashTable(Table,__real_size,ProcAllocate) \
  }
  ```

## constants.h

- Memory allocation constants are defined with the assumption of 32-bit addressing:
  ```
  #define AKLBLOCKSIZE	(200 * 1024)
  #define AKLHEAPLIMIT	8
  #define AKLCONSTSIZE  	(400 * 1024)
  #define AKLCODESIZE	(1000 * 1024)
  ```

## gc.c

- The garbage collection system assumes 32-bit pointers throughout the implementation, particularly in the marking and forwarding of pointers.

- **Lines 126-130**: Stack setup assumes 32-bit word boundaries:
  ```
  #define SetupStack(TYPE,STACK,END,TOP,SIZE) \
  {\
      if ((END) == NULL) {\
        (STACK) = (TYPE*) malloc(sizeof(TYPE)*(SIZE));\
        if ((STACK) == NULL) {\
  ```

## Summary

The emulator codebase has significant dependencies on 32-bit architecture, particularly in:

1. The tagged pointer scheme which is explicitly designed for 32-bit words
2. Memory alignment and addressing assumptions
3. Bit manipulation operations
4. Register allocation in the core engine
5. Numeric operations, especially in bignum handling
6. Garbage collection pointer manipulation

Porting to a 64-bit architecture would require substantial changes to these components, with special attention to the tagged pointer scheme which is fundamental to the system's operation.
