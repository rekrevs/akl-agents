# WARNING: This Makefile is meant to be invoked via the AGENTS root Makefile.
# Direct invocation may not work, except that make X.dvi or X.ps should work
# for X in
# aklintro engine internals library user
# assuming the required utilities are on hand.

SHELL = /bin/sh

MAKEINFO = makeinfo
MAKEINFOFLAGS = --fill-column=70


INFO =	agents-user \
	agents-library

INFO_MENU =	"* AGENTS User Manual: (agents-user)." \
		"* AGENTS Library Manual: (agents-library)."

DVI = 	aklintro.dvi \
	library.dvi \
	user.dvi

PS = 	aklintro.ps \
	library.ps \
	user.ps

LIB = ../library

LIB_TEXI =	library/begin.texi \
		$(LIB)/assoc.texi \
		library/gm.texi \
		$(LIB)/lists.texi \
		$(LIB)/ordsets.texi \
		library/prolog.texi \
		$(LIB)/random.texi \
		library/reader.texi \
		library/saga.texi \
		$(LIB)/ugraphs.texi \
		library/end.texi


info:	$(INFO)

agents-user:	user.texi
	$(MAKEINFO) $(MAKEINFOFLAGS) user.texi

agents-library:	library.texi
	$(MAKEINFO) $(MAKEINFOFLAGS) library.texi


dvi:	$(DVI)


ps:	$(PS)


.SUFFIXES:	.akl .dvi .ps .tex .texi

.akl.texi:
	sed -n "s/%i[ \t]//p" < $< > $@

.tex.dvi:
	latex $*.tex
	bibtex $*
	latex $*.tex
	latex $*.tex

.tex.ps:
	latex $*.tex
	bibtex $*
	latex $*.tex
	latex $*.tex
	dvips $*.dvi -o

.texi.dvi:
	tex $*.texi
	texindex $*.cp $*.fn $*.ky $*.pg $*.tp $*.vr
	tex $*.texi

.texi.ps:
	tex $*.texi
	texindex $*.cp $*.fn $*.ky $*.pg $*.tp $*.vr
	tex $*.texi
	dvips $*.dvi -o

library.texi:	$(LIB_TEXI)
	cat $(LIB_TEXI) > library.texi
	./update-links library.texi

aklintro.dvi:
	(cd aklintro; \
	 $(MAKE) $(MFLAGS) dvi)

aklintro.ps:
	(cd aklintro; \
	 $(MAKE) $(MFLAGS) ps)


install:
	for info in $(INFO); do \
	  $(INSTALL_DATA) $$info* $(infodir); \
	done
	for info_menu in $(INFO_MENU); do \
	  echo "" >> $(infodir)/dir; \
	  echo "$$info_menu" >> $(infodir)/dir; \
	done


uninstall:
	for info in $(INFO); do \
	   rm -f $(infodir)/`basename $$info`*; \
	done
# At present, $(infodir)/dir is not cleaned up.  This is the place to do it.


clean:
	rm -f *.aux *.bbl *.blg *.cp *.cps *.fn *.fns *.ky *.kys *.log *.pg \
	      *.pgs *.toc *.tp *.tps *.vr *.vrs $(LIB)/*.texi library.texi \
	      *.dvi *.ps
	(cd aklintro; \
	 $(MAKE) $(MFLAGS) clean)


realclean:	clean
	rm -f agents-user* agents-library*
	(cd aklintro; \
	 $(MAKE) $(MFLAGS) realclean)


TEXI2HTML = texi2html

html:   user.texi library.texi
	$(TEXI2HTML) -split_chapter -menu -glossary user.texi
	$(TEXI2HTML) -split_chapter -menu -glossary library.texi
