# WARNING: This Makefile is meant to be invoked via the AGENTS root Makefile.
# Direct invocation may not work.

SHELL = /bin/sh

AGENTS = ../../agents


READER = ../reader.pam

OBJ =	annotDPROGRAM.pam \
	convDPROGRAMtoDPROGRAM.pam \
	convDTERM0toDTERM1.pam \
	convDTERM1toDPROGRAM.pam \
	rwrDTERM0.pam \
	rwrDTERM1.pam \
	syntax.pam 

SRC =	annotDPROGRAM.akl \
	convDPROGRAMtoDPROGRAM.akl \
	convDTERM0toDTERM1.akl \
	convDTERM1toDPROGRAM.akl \
	rwrDTERM0.akl \
	rwrDTERM1.akl \
	syntax.akl 


all:	$(READER)

$(READER):	$(OBJ)
	cat $(OBJ) > $(READER)

.SUFFIXES:	.pam .akl

.akl.pam:	
	echo "compilef('$*')." | $(AGENTS)


install:
	$(INSTALL_DATA) $(READER) $(librarydir)


uninstall:
	rm -f $(librarydir)/`basename $(READER)`


clean: 
	rm -f $(READER) $(OBJ)


realclean:	clean



# The following stuff is for use at SICS only.

PROGRAM = toplevel

#AGENTSCOMMAND = xref(testfile)
AGENTSCOMMAND = normalize_program2(testfile)

#AGENTSDEBUGCMD = failcatch, trace_set_print_depth(-1), $(AGENTSCOMMAND)
AGENTSDEBUGCMD = failcatch, $(AGENTSCOMMAND)


testfile: $(SRC)
	cp p2 testfile

bigtestfile: $(SRC) 
	cat $(SRC) ../lists.akl ../ordsets.akl ../ugraphs.akl ../assoc.akl >testfile

test: testfile $(PROGRAM).pam test-examples
	echo "ok"

test-examples: $(PROGRAM).pam $(READER)
	echo "load(library(lists)), load(library(ordsets)), load(library(ugraphs)), load(library(assoc)), load(library(reader)),load(library(map)), load(library(utilities)), load(library(timing)), load($(PROGRAM)),$(AGENTSCOMMAND)." | $(AGENTS)


debug:  testcmd
	echo "query0:- load(library(lists)), load(library(ordsets)), load(library(ugraphs)), load(library(assoc)), load(library(reader)), load(library(map)), load(library(utilities)), load(library(timing)), load($(PROGRAM)). query :- query0, $(AGENTSDEBUGCMD)." >query.akl; echo "##### run agents -t with: compile(query), query. "


testcmd: $(PROGRAM).pam $(READER)
	echo "The used files were compiled"
