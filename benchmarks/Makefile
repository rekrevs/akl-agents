# $Id: Makefile,v 1.2 1994/01/28 14:35:17 jm Exp $
# Copyright (C) 1990, Swedish Institute of Computer Science.

PAM=    bench1.pam

COMP= ../akl

TMPFILE= %%%make.tmp

.SUFFIXES: .pam .akl

.akl.pam:	
	echo "compilef($*,flat)." >> $(TMPFILE)

all:
	/bin/rm -f $(TMPFILE)
	touch $(TMPFILE)
	$(MAKE) $(MFLAGS) pam
	cat $(TMPFILE) | $(COMP)
	/bin/rm -f $(TMPFILE)

pam: $(PAM)

clean:
	/bin/rm -f $(PAM)

realclean:	clean
