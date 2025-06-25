# $Id: Makefile,v 1.2 1993/05/28 16:25:00 perbrand Exp $
# Copyright (C) 1990, Swedish Institute of Computer Science.

RESULT= comp.pam

PAM= \
alloc_help.pam      pred_non_unif.pam   non_unif.pam   test.pam \
body_alloc.pam      opt.pam             pred_sus.pam        transform.pam \
builtin.pam         opt_help.pam        pred_switch.pam     transform_help.pam\
clause.pam          opt_reg.pam         pred_tree.pam       unif.pam \
compiler.pam        opt_try.pam         push.pam            var.pam \
help.pam            pred.pam            spec_alloc.pam      opt_block.pam\
io.pam              pred_def.pam        start.pam           head_alloc.pam

COMP= ../akl

TMPFILE= %%%make.tmp

.SUFFIXES: .pam .akl

.akl.pam:	
	echo "compilef($*)." >> $(TMPFILE)

all:
	/bin/rm -f $(TMPFILE)
	touch $(TMPFILE)
	$(MAKE) $(MFLAGS) pam
	cat $(TMPFILE) | $(COMP)
	/bin/rm -f $(TMPFILE)
#	/bin/rm -f $(RESULT)
#	cat $(PAM) > $(RESULT)

pam: $(PAM)

clean:
	/bin/rm -f $(PAM)

realclean:	clean
