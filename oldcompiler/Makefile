# WARNING: This Makefile is meant to be invoked via the AGENTS root Makefile.
# Direct invocation may not work.

SHELL = /bin/sh

AGENTS = ../agents


# Variables to pass to all submakes apart from installs, uninstalls, and
# cleans.
VARS_TO_PASS = \
	AGENTS="$(AGENTS)" \
	AGENTSFLAGS="$(AGENTSFLAGS)"


COMPILER = comp.pam

SRC =	allocate.akl \
	analyze.akl \
	aux.akl \
	body.akl \
	builtin.akl \
	clause.akl \
	compiler.akl \
	edit.akl \
	fd_akl.akl \
	fd_arith_ask.akl \
	fd_arith_compile.akl \
	fd_arith_tell.akl \
	fd_mon.akl \
	fd_range.akl \
	graph.akl \
	head.akl \
	instr.akl \
	main.akl \
	modes.akl \
	parser.akl \
	reader.akl \
	try.akl \
	unify.akl \
	writepam.akl

OBJ =	allocate.pam \
	analyze.pam \
	aux.pam \
	body.pam \
	builtin.pam \
	clause.pam \
	compiler.pam \
	edit.pam \
	fd_akl.pam \
	fd_arith_ask.pam \
	fd_arith_compile.pam \
	fd_arith_tell.pam \
	fd_mon.pam \
	fd_range.pam \
	graph.pam \
	head.pam \
	instr.pam \
	main.pam \
	modes.pam \
	parser.pam \
	reader.pam \
	try.pam \
	unify.pam \
	writepam.pam


compiler:	$(COMPILER)

$(COMPILER):	$(SRC)
	$(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' $(OBJ)
	cat $(OBJ) > $(COMPILER)

.SUFFIXES:	.pam .akl

.akl.pam:
	echo "compilef('$*')." | $(AGENTS)


install:
	$(INSTALL_DATA) $(COMPILER) $(compilerdir)


uninstall:
	rm -f $(compilerdir)/`basename $(COMPILER)`


clean:


realclean:	clean
	rm -f $(COMPILER) $(OBJ)
