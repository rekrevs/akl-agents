/* $Id: regdefs.h,v 1.1 1993/05/11 11:58:19 bd Exp $ */

/* Compile with -DHARDREGS to enable this */

#if defined(__GNUC__) && defined(HARDREGS)

#ifdef mips
#define REGISTER1 asm("$23")
#define REGISTER2 asm("$22")
#define REGISTER3 asm("$21")
#define REGISTER4 asm("$20")
#define REGISTER5 asm("$19")
#define REGISTER6 asm("$18")
#define REGISTER7 asm("$17")
#define REGISTER8 asm("$16")
#define REGISTER9 asm("$15")
#define REGISTER10 asm("$14")
#endif

#ifdef sparc
#define REGISTER1 asm("%l7")
#define REGISTER2 asm("%l6")
#define REGISTER3 asm("%l5")
#define REGISTER4 asm("%l4")
#define REGISTER5 asm("%l3")
#define REGISTER6 asm("%l2")
#define REGISTER7 asm("%i5")
#define REGISTER8 asm("%i4")
#define REGISTER9 asm("%i3")
#endif

#ifdef alpha
#define REGISTER1 asm("$23")
#define REGISTER2 asm("$22")
#define REGISTER3 asm("$21")
#define REGISTER4 asm("$20")
#define REGISTER5 asm("$19")
#define REGISTER6 asm("$18")
#define REGISTER7 asm("$17")
#define REGISTER8 asm("$16")
#define REGISTER9 asm("$15")
#define REGISTER10 asm("$14")
#endif

#if defined(__x86_64__) || defined(__amd64__)
#define REGISTER1 asm("r15")
#define REGISTER2 asm("r14")
#define REGISTER3 asm("r13")
#define REGISTER4 asm("r12")
#define REGISTER5 asm("rbx")
#define REGISTER6 asm("rbp")
#define REGISTER7 asm("r11")
#define REGISTER8 asm("r10")
#define REGISTER9 asm("r9")
#define REGISTER10 asm("r8")
#endif

#endif

#ifndef REGISTER10
#define REGISTER10
#endif

#ifndef REGISTER9
#define REGISTER9
#endif

#ifndef REGISTER8
#define REGISTER8
#endif

#ifndef REGISTER7
#define REGISTER7
#endif

#ifndef REGISTER6
#define REGISTER6
#endif

#ifndef REGISTER5
#define REGISTER5
#endif

#ifndef REGISTER4
#define REGISTER4
#endif

#ifndef REGISTER3
#define REGISTER3
#endif

#ifndef REGISTER2
#define REGISTER2
#endif

#ifndef REGISTER1
#define REGISTER1
#endif

