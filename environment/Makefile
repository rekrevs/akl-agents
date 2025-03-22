# WARNING: This Makefile is meant to be invoked via the AGENTS root Makefile.
# Direct invocation may not work.

SHELL = /bin/sh

AGENTS = ../agents


# Variables to pass to all submakes apart from installs, uninstalls, and
# cleans.
VARS_TO_PASS = \
	AGENTS="$(AGENTS)" \
	AGENTSFLAGS="$(AGENTSFLAGS)"


ENVIRONMENT = boot.pam

SRC =	aggregate.akl \
	builtin.akl \
	current_op.akl \
	error.akl \
	fd_arith.akl \
	fd_atmost.akl \
	fd_count.akl \
	fd_element.akl \
	fd_labeling.akl \
	foreign.akl \
	format.akl \
	functor.akl \
	load.akl \
	prolog.akl \
	qa.akl \
	rdtok.akl \
	read.akl \
	reflection.akl \
	sort.akl \
	statistics.akl \
	streams.akl \
	synch3.akl \
	unix.akl \
	write.akl

OBJ =	aggregate.pam \
	builtin.pam \
	current_op.pam \
	error.pam \
	fd_arith.pam \
	fd_atmost.pam \
	fd_count.pam \
	fd_element.pam \
	fd_labeling.pam \
	foreign.pam \
	format.pam \
	functor.pam \
	load.pam \
	prolog.pam \
	qa.pam \
	rdtok.pam \
	read.pam \
	reflection.pam \
	sort.pam \
	statistics.pam \
	streams.pam \
	synch3.pam \
	unix.pam \
	write.pam


environment:	$(ENVIRONMENT)

$(ENVIRONMENT):	$(SRC)
	$(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' $(OBJ)
	cat $(OBJ) > $(ENVIRONMENT)

.SUFFIXES:	.pam .akl

.akl.pam:
	echo "compilef('$*')." | $(AGENTS)


install:
	$(INSTALL_DATA) $(ENVIRONMENT) $(environmentdir)


uninstall:
	rm -f $(environmentdir)/`basename $(ENVIRONMENT)`


clean:


realclean:	clean
	rm -f $(ENVIRONMENT) $(OBJ)
