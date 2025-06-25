SHELL = /bin/sh


dvi:	aklintro.dvi
	mv aklintro.dvi ..

ps:	aklintro.ps
	mv aklintro.ps ..

.SUFFIXES:	.dvi .ps .tex

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


clean:
	rm -f *.aux *.bbl *.blg *.log *.toc *.dvi


realclean:	clean
