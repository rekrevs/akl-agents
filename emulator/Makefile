# WARNING: This Makefile is meant to be invoked via the AGENTS root Makefile.
# Direct invocation may not work.

SHELL = /bin/sh

BISON = bison
CC = gcc
CFLAGS = -g -fgnu89-inline
ETAGS = etags
FLEX = flex
FLEXFLAGS = -8
LDFLAGS = -lm
M4 = m4

AGENTSFLAGS = -DMETERING -DNDEBUG -DNOBIGNUM


OBJ =	abstraction.o \
	aggregate.o \
	array.o \
	bam.o \
	bignum.o \
	builtin.o \
	candidate.o \
	code.o \
	compare.o \
	config.o \
	copy.o \
	debug.o \
	display.o \
	engine.o \
	error.o \
	examine.o \
	exstate.o \
	fd.o \
	fd_akl.o \
	foreign.o \
	functor.o \
	gc.o \
	ghash.o \
	initial.o \
	inout.o \
	intrpt.o \
	is.o \
	load.o \
	main.o \
	names.o \
	parser.tab.o \
	parser.yy.o \
	port.o \
	predicate.o \
	ptrhash.o \
	rational.o \
	reflection.o \
	statistics.o \
	storage.o \
	time.o \
	trace.o \
	unify.o

DEF =   abstraction.h \
	aggregate.h \
	array.h \
	bam.h \
	basics.h \
	bignum.h \
	builtin.h \
	c.h \
	candidate.h \
	code.h \
	compare.h \
	config.h \
	constants.h \
	copy.h \
	debug.h \
	decodeinstr.h \
	display.h \
	encodeinstr.h \
	engine.h \
	error.h \
	examine.h \
	exstate.h \
	fd.h \
	fd_akl.h \
	fd_instr.h \
	foreign.h \
	functor.h \
	gc.h \
	ghash.h \
	hash.h \
	include.h \
	initial.h \
	inout.h \
	instrdefs.h \
	instructions.h \
	intrpt.h \
	is.h \
	load.h \
	names.h \
	opcodes.h \
	parser.tab.h \
	port.h \
	predicate.h \
	ptrhash.h \
	rational.h \
	reflection.h \
	regdefs.h \
	runtime.h \
	statistics.h \
	storage.h \
	sysdeps.h \
	term.h \
	time.h \
	trace.h \
	tree.h \
	unify.h


agents:	agents.o foreign_stub.o
	${CC} $(CFLAGS) $(AGENTSFLAGS) agents.o foreign_stub.o $(LDFLAGS) \
	      -o agents

agents.o:	$(OBJ)
	ld -r -o agents.o $(OBJ)

$(OBJ):	$(DEF)

.c.o:
	$(CC) -c $(CFLAGS) $(AGENTSFLAGS) $< -o $@

parser.tab.c parser.tab.h:	parser.y 
	$(BISON) -d $(BISONFLAGS) parser.y

parser.yy.c:	parser.l parser.tab.h
	$(FLEX) $(FLEXFLAGS) parser.l 
	mv lex.yy.c parser.yy.c

parser.y:	parser.y.m4
	$(M4) $(M4FLAGS) parser.y.m4 > parser.y

display.c:	display.c.m4
	$(M4) $(M4FLAGS) display.c.m4 > display.c


clean:
	rm -f agents agents.o foreign_stub.o $(OBJ) *.d *.tcov


realclean:	clean
	rm -f parser.tab.c parser.tab.h parser.yy.c parser.y display.c TAGS


TAGS:
	$(ETAGS) $(ETAGSFLAGS) *.c *.h
