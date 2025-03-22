# WARNING: This Makefile is meant to be invoked via the AGENTS root Makefile.
# Direct invocation may not work.

SHELL = /bin/sh

AGENTS = ../agents

# Variables to pass to all submakes apart from installs, uninstalls, and
# cleans.
VARS_TO_PASS = \
	AGENTS="$(AGENTS)" \
	AGENTSFLAGS="$(AGENTSFLAGS)"


OBJ =	assoc.pam \
	lists.pam \
	map.pam \
	ordsets.pam \
	random.pam \
	timing.pam \
	ugraphs.pam \
	utilities.pam


all:	$(OBJ) make-gm make-reader make-saga

.SUFFIXES:	.pam .akl

.akl.pam:	
	echo "compilef('$*')." | $(AGENTS)

make-gm:
	(cd gm; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' all)

make-reader:
	(cd readerdir; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' all)

make-saga:
	(cd sagadir; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' all)


install: install-gm install-reader install-saga
	for obj in $(OBJ); do \
	  $(INSTALL_DATA) $$obj $(librarydir); \
	done

install-gm:
	(cd gm; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' install)

install-reader:
	(cd readerdir; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' install)

install-saga:
	(cd sagadir; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' install)


uninstall: uninstall-gm uninstall-reader uninstall-saga
	for obj in $(OBJ); do \
	  rm -f $(librarydir)/`basename $$obj`; \
	done

uninstall-gm:
	(cd gm; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' uninstall)

uninstall-reader:
	(cd readerdir; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' uninstall)

uninstall-saga:
	(cd sagadir; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' uninstall)


clean:
	rm -f $(OBJ)
	(cd gm; \
	 $(MAKE) $(MFLAGS) clean)
	(cd readerdir; \
	 $(MAKE) $(MFLAGS) clean)
	(cd sagadir; \
	 $(MAKE) $(MFLAGS) clean)


realclean:	clean
	(cd gm; \
	 $(MAKE) $(MFLAGS) realclean)
	(cd readerdir; \
	 $(MAKE) $(MFLAGS) realclean)
	(cd sagadir; \
	 $(MAKE) $(MFLAGS) realclean)
