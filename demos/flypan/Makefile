# WARNING: This Makefile is meant to be invoked via the AGENTS root Makefile.
# Direct invocation may not work.

SHELL = /bin/sh

AGENTS = ../../agents


FLYPAN = ../flypan.pam

OBJ =	gpan.pam \
	gpan_graph.pam \
	gpan_query.pam


all:	$(FLYPAN)

$(FLYPAN):	$(OBJ)
	cat $(OBJ) > $(FLYPAN)

.SUFFIXES:	.pam .akl

.akl.pam:	
	echo "compilef('$*')." | $(AGENTS)


clean:
	rm -f $(FLYPAN) $(OBJ)


realclean:	clean
