# WARNING: This Makefile is meant to be invoked via the AGENTS root Makefile.
# Direct invocation may not work.

SHELL = /bin/sh

AGENTS = ../../agents


GM = ../gm.pam

OBJ =	gmcreate.pam \
	gmdb.pam \
	gmio.pam \
	gmmessage.pam \
	gmstart.pam


all:	$(GM)

$(GM):	$(OBJ)
	cat $(OBJ) > $(GM)

.SUFFIXES:	.pam .akl

.akl.pam:	
	echo "compilef('$*')." | $(AGENTS)


install:
	$(INSTALL_DATA) $(GM) $(librarydir)


uninstall:
	rm -f $(librarydir)/`basename $(GM)`


clean:
	rm -f $(GM) $(OBJ)


realclean:	clean
