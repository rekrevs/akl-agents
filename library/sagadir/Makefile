# WARNING: This Makefile is meant to be invoked via the AGENTS root Makefile.
# Direct invocation may not work.

SHELL = /bin/sh

AGENTS = ../../agents


SAGA =  ../saga.pam

SAGARUNTIME =  ../sagaruntime.pam

SAGAOBJ =	confldiag.pam \
		dfa.pam \
		front.pam \
		gram.pam \
		main.pam \
		min.pam \
		misc.pam \
		pack.pam \
		repr.pam \
		symbol.pam

SAGARUNTIMEOBJ =	fskel.pam \
			fskel2.pam 


all:	$(SAGA) $(SAGARUNTIME)

$(SAGA):	$(SAGAOBJ)
	cat $(SAGAOBJ) > $(SAGA)

$(SAGARUNTIME):	$(SAGARUNTIMEOBJ)
	cat $(SAGARUNTIMEOBJ) > $(SAGARUNTIME)

.SUFFIXES:	.pam .akl

.akl.pam:	
	echo "compilef('$*')." | $(AGENTS)


install:
	$(INSTALL_DATA) $(SAGA) $(librarydir)
	$(INSTALL_DATA) $(SAGARUNTIME) $(librarydir)


uninstall:
	rm -f $(librarydir)/`basename $(SAGA)` \
	      $(librarydir)/`basename $(SAGARUNTIME)`


clean:
	rm -f $(SAGA) $(SAGARUNTIME) $(SAGAOBJ) $(SAGARUNTIMEOBJ)


realclean:	clean
