# WARNING: This Makefile is meant to be invoked via the AGENTS root Makefile.
# Direct invocation may not work.

SHELL = /bin/sh

AGENTS = ../agents

# Variables to pass to all submakes apart from installs, uninstalls, and
# cleans.
VARS_TO_PASS = \
	AGENTS="$(AGENTS)" \
	AGENTSFLAGS="$(AGENTSFLAGS)"


OBJ =	ancestors.pam \
	cipher.pam \
	cube.pam \
	fd_alpha.pam \
	fd_cars.pam \
	fd_eq10.pam \
	fd_eq20.pam \
	fd_five.pam \
	fd_magic.pam \
	fd_queens.pam \
	fd_test.pam \
	flypan.pam \
	ghcexamples.pam \
	gmdemo.pam \
	kkqueen.pam \
	klife.pam \
	knights.pam \
	life.pam \
	lists.pam \
	lookup.pam \
	lsqueen.pam \
	mastermind.pam \
	member.pam \
	merge.pam \
	money.pam \
	mwmerge.pam \
	orp.pam \
	pascal.pam \
	prove.pam \
	puzzle.pam \
	qsort_data.pam \
	queens.pam \
	scanner.pam \
	semigroup.pam \
	sublist.pam \
	test.pam \
	triangle.pam \
	turtles.pam \
	waves.pam \
	xlife.pam \
	xscanner.pam \
	zebra.pam


all:	$(OBJ)

.SUFFIXES:	.pam .akl

.akl.pam:	
	echo "compilef('$*')." | $(AGENTS)

flypan.pam:
	(cd flypan; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' all)


install:
# This cp is clumsy but it works.
# Exclude flypan at present.
	cp *.akl $(demosdir)
	for obj in $(OBJ); do \
	  if [ $$obj != flypan.pam ]; then \
	    $(INSTALL_DATA) $$obj $(demosdir); \
	  fi; \
	done


uninstall:
# This rm is clumsy but it works, as long as no .akl files have been added to
# $(demosdir) since installation.
	rm -f $(demosdir)/*.akl
	for obj in $(OBJ); do \
	  rm -f $(demosdir)/`basename $$obj`; \
	done


clean:
	rm -f $(OBJ)
	(cd flypan; \
	 $(MAKE) $(MFLAGS) clean)


realclean:
	(cd flypan; \
	 $(MAKE) $(MFLAGS) realclean)
