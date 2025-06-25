dnl config.m4.  Generated automatically by configure.
changequote(<,>)dnl
ifdef(<__CONFIG_M4_INCLUDED__>,,<
define(<TEXT>, <.text>)
define(<DATA>, <.data>)
define(<GLOBL>, <.globl>)
define(<LABEL_SUFFIX>, <$1:>)
define(<TYPE>, <.type	$1,#$2>)
define(<SIZE>, <.size	$1,$2>)
define(<LSYM_PREFIX>, <.L>)
define(<W32>, <.long>)
define(<GSYM_PREFIX>, <>)
define(<ALIGN_LOGARITHMIC>,<no>)
dnl  CONFIG_TOP_SRCDIR is a path from the mpn builddir to the top srcdir
define(<CONFIG_TOP_SRCDIR>,<`../.'>)
define(<KARATSUBA_SQR_THRESHOLD>,<59>)
>)
changequote(`,')dnl
ifdef(`__CONFIG_M4_INCLUDED__',,`
include(CONFIG_TOP_SRCDIR`/mpn/asm-defs.m4')
define_not_for_expansion(`HAVE_TARGET_CPU_sparcv9')
')
define(`__CONFIG_M4_INCLUDED__')
