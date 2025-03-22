# Generated automatically from Makefile.in by configure.
# targets for users:
#   all		Build AGENTS in $(srcdir), which should contain the distributed
#		files or symbolic links to them, as created by mkbuilddirs; see
#		README.  This is the default target.  It does not build .dvi or
#		.ps files; see dvi and ps below.  Note that the distribution
#		includes info files.
#   install	Install AGENTS in $(prefix).  By default, pieces of AGENTS are
#		installed as follows:
#		  agents executable	$(prefix)/bin
#		  info files		$(prefix)/info
#		  man page		$(prefix)/man/man1
#		  all else		$(prefix)/lib/agents$(VERSION)
#		You can override these on the make command line by setting the
#		variables $(bindir) etc. set by default at ***** below.  These
#		directories are created if necessary.  Naturally, you must have
#		write access to them.  make install assumes the necessary files
#		are available, so do make all beforehand, and if you have done
#		make realclean (or otherwise removed the info files), also do
#		make info beforehand.
#   uninstall	Uninstall AGENTS in $(prefix).  This removes anything make
#		install would replace.  REQUIRES basename.
#		At present, the directory for info files is not cleaned up.
#   clean	Remove anything make all, check, or check-coverage created from
#		the distribution.  This preserves the distribution and anything
#		configure created, tags tables, and .dvi and .ps files.
#   mostlyclean	Like clean, but may preserve things people usually do not wish
#		to rebuild, e.g., because rebuilding is time-consuming.
#   distclean	Like clean, but also remove anything configure created.
#   realclean	Remove anything that could be rebuilt according to the AGENTS
#		Makefiles.  THIS REMOVES THINGS IN THE DISTRIBUTION, SO NEVER
#		DO THIS UNLESS YOU HAVE THE FOLLOWING UTILITIES, NECESSARY TO
#		REBUILD THE DISTRIBUTION, IN ADDITION TO A WORKING COPY OF THE
#		CURRENT VERSION OF AGENTS ITSELF:
#		bison flex m4 makeinfo
#   TAGS	Build a tags table in the emulator directory.  REQUIRES etags.
#   info	Rebuild info files in the doc directory.  REQUIRES makeinfo.
#   dvi		Build .dvi files in the doc directory.  REQUIRES latex, bibtex,
#		tex, AND texindex.
#   ps		Build .ps files in the doc directory.  REQUIRES latex, bibtex,
#		tex, texindex, AND dvips.
#   dist	Create .tar.Z file of AGENTS distribution.  FIXME: Add details.
#   check	Test ./agents.  REQUIRES DejaGnu (runtest).
#   check-coverage
#		Test ./agents and profile test coverage of emulator directory.
#		REQUIRES DejaGnu (runtest) AND tcov.  The following subtargets
#		may be useful occasionally:
#		  make-tcovagents Build an instance of AGENTS supporting tcov.
#		  test-tcovagents Test this instance using DejaGnu.
#		  tcov-tcovagents Profile test coverage using tcov.
#   eagents	TEMPORARY.  Build eagents in emulator directory.
#   oagents	TEMPORARY.  Build oagents in emulator directory.
#   tagents	TEMPORARY.  Build tagents in emulator directory.

VERSION = 0.9

SHELL = /bin/sh

# $(srcdir) MUST BE ABSOLUTE.
srcdir = /home/sverker/agents
gmpdir = $(srcdir)/gmp

# ***** INSTALLATION VARIABLES *****
# $(prefix) MUST BE ABSOLUTE.
# GMP is not installed publicly at present; this is controlled by $(libdir) and
# $(includedir).  Note that the uninstall-gmp target assumes $(libdir) and
# $(includedir) are not public, so it is safe to remove anything from them, and
# $(oldincludedir) is the same as $(includedir), so it is unnecessary to remove
# anything from it.
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib/agents$(VERSION)/lib
includedir = $(prefix)/lib/agents$(VERSION)/include
oldincludedir = $(includedir)
datadir = $(prefix)/lib/agents$(VERSION)
compilerdir = $(datadir)/compiler
demosdir = $(datadir)/demos
emulatordir = $(datadir)/emulator
environmentdir = $(datadir)/environment
librarydir = $(datadir)/library
mandir = $(prefix)/man/man1
manext = 1
infodir = $(prefix)/info

INSTALL = /usr/gnu/bin/install -c
INSTALL_DATA = $(INSTALL) -m 644
INSTALL_PROGRAM = $(INSTALL)

AGENTS = $(srcdir)/agents
BISON = bison
CC = gcc
CFLAGS = -O2
ALL_CFLAGS = -I$(gmpdir) $(CFLAGS)
ETAGS = etags
FLEX = flex
FLEXFLAGS = -8
LDFLAGS = -L$(gmpdir) -lgmp -lm
M4 = m4
MAKE = make
MAKEINFO = makeinfo
MAKEINFOFLAGS = --fill-column=70
RUNTEST = runtest
WORKINGCOPY = agents

AGENTSFLAGS_AUTO =  
# E for Environment field in records
EAGENTSFLAGS = $(AGENTSFLAGS_AUTO) -DMETERING -DNDEBUG -DSTRUCT_ENV
# O for Ordinary
OAGENTSFLAGS = $(AGENTSFLAGS_AUTO) -DMETERING -DNDEBUG
# S for Script (used in the agents script)
SAGENTSFLAGS = $(AGENTSFLAGS_AUTO) -DNDEBUG -I$(srcdir)/emulator
# T for Trace
TAGENTSFLAGS = $(AGENTSFLAGS_AUTO) -DMETERING -DNDEBUG -DTRACE -g

# Variables to pass to all submakes apart from installs, uninstalls, and cleans
# (except of gmp, which is a special case, regretably).
VARS_TO_PASS = \
	AGENTS="$(AGENTS)" \
	AGENTSFLAGS="$(AGENTSFLAGS)" \
	BISON="$(BISON)" \
	BISONFLAGS="$(BISONFLAGS)" \
	CC="$(CC)" \
	CFLAGS="$(ALL_CFLAGS)" \
	ETAGS="$(ETAGS)" \
	ETAGS="$(ETAGSFLAGS)" \
	FLEX="$(FLEX)" \
	FLEXFLAGS="$(FLEXFLAGS)" \
	LDFLAGS="$(LDFLAGS)" \
	M4="$(M4)" \
	M4FLAGS="$(M4FLAGS)" \
	MAKE="$(MAKE)" \
	MFLAGS="$(MFLAGS)" \
	MAKEINFO="$(MAKEINFO)" \
	MAKEINFOFLAGS="$(MAKEINFOFLAGS)"

# Variables to pass to all submake installs and uninstalls.
INSTALL_VARS_TO_PASS = \
	INSTALL="$(INSTALL)" \
	INSTALL_DATA="$(INSTALL_DATA)" \
	INSTALL_PROGRAM="$(INSTALL_PROGRAM)" \
	bindir=$(bindir) \
	libdir=$(libdir) \
	includedir=$(includedir) \
	oldincludedir=$(oldincludedir) \
	datadir=$(datadir) \
	compilerdir=$(compilerdir) \
	demosdir=$(demosdir) \
	emulatordir=$(emulatordir) \
	environmentdir=$(environmentdir) \
	librarydir=$(librarydir) \
	mandir=$(mandir) \
	manext=$(manext) \
	infodir=$(infodir)


all: 	make-gmp make-emulator make-compiler make-environment make-version \
	make-agents make-library make-tools make-demos

make-gmp:
	(cd gmp; \
	 $(MAKE) $(MFLAGS) CC=$(CC) libgmp.a)

make-emulator:
	(cd emulator; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' \
		 AGENTSFLAGS="$(OAGENTSFLAGS)" agents; \
	 mv agents.o oagents.o; \
	 mv agents oagents)

make-compiler:
	(cd oldcompiler; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) AGENTS="$(WORKINGCOPY)" \
		 VARS_TO_PASS='$(VARS_TO_PASS) AGENTS="$(WORKINGCOPY)"' \
		 compiler)

make-environment:
	(cd environment; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) AGENTS="$(WORKINGCOPY)" \
		 VARS_TO_PASS='$(VARS_TO_PASS) AGENTS="$(WORKINGCOPY)"' \
		 environment)

make-version:
	echo \
"% WARNING: Do not edit this file; version.pam.in is the source file." \
	     > version.pam
	sed -e "s%--VERSION--%$(VERSION)%" \
	    -e "s%--DATE--%`date`%" \
	    -e "s%--DEMOSDIR--%$(srcdir)/demos%" \
	    -e "s%--LIBRARYDIR--%$(srcdir)/library%" \
	    version.pam.in >> version.pam

make-agents:	agents.in
	echo \
"#! /bin/csh -f" \
	     > agents
	echo \
"# WARNING: Do not edit this file; agents.in is the source file." \
	     >> agents
	sed -e "s%##%  %" \
	    -e "s%--AGENTSEXE--%$(srcdir)/emulator/oagents%" \
	    -e "s%--AGENTSOBJ--%$(srcdir)/emulator/oagents.o%" \
	    -e "s%--COMPILERDIR--%$(srcdir)/oldcompiler%" \
	    -e "s%--EMULATORDIR--%$(srcdir)/emulator%" \
	    -e "s%--ENVIRONMENTDIR--%$(srcdir)/environment%" \
	    -e "s%--VERSION--%$(srcdir)/version.pam%" \
	    -e "s%--CC--%$(CC)%" \
	    -e "s%--CFLAGS--%$(ALL_CFLAGS)%" \
	    -e "s%--LDFLAGS--%$(LDFLAGS)%" \
	    -e "s%--AGENTSFLAGS--%$(SAGENTSFLAGS)%" \
	    agents.in >> agents
	chmod +x agents

make-library:
	(cd library; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' all)

make-tools:
	(cd tools; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' all)

make-demos:
	(cd demos; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' all)


install:	installdirs install-gmp install-emulator install-compiler \
		install-environment install-version install-agents \
		install-library install-tools install-demos install-man \
		install-info install-emacs-mode

installdirs:
	$(srcdir)/mkinstalldirs $(bindir) \
				$(libdir) \
				$(includedir) \
				$(oldincludedir) \
				$(datadir) \
				$(compilerdir) \
				$(demosdir) \
				$(emulatordir) \
				$(environmentdir) \
				$(librarydir) \
				$(mandir) \
				$(infodir)

# Regretably, the GMP Makefile has no install target.
# *.h is probably overkill.
install-gmp:
	(cd gmp; \
	 $(INSTALL_PROGRAM) libgmp.a $(libdir); \
	 $(INSTALL_DATA) *.h $(includedir); \
	 $(INSTALL_DATA) *.h $(oldincludedir))

# *.h is definitely overkill.
install-emulator:
	(cd emulator; \
	 $(INSTALL_PROGRAM) oagents.o $(bindir)/agents.o; \
	 $(INSTALL_PROGRAM) oagents $(bindir)/agents.e; \
	 $(INSTALL_DATA) *.h $(emulatordir))

install-compiler:
	(cd oldcompiler; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' install)

install-environment:
	(cd environment; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' install)

install-version:
	sed -e "s%--VERSION--%$(VERSION)%" \
	    -e "s%--DATE--%`date`%" \
	    -e "s%--DEMOSDIR--%$(demosdir)%" \
	    -e "s%--LIBRARYDIR--%$(librarydir)%" \
	    version.pam.in > version.pam.tmp
	$(INSTALL_DATA) version.pam.tmp $(datadir)/version.pam
	rm version.pam.tmp

install-agents:	agents.in
	echo \
"#! /bin/csh -f" \
	     > agents.tm1
	sed -e "s%--AGENTSEXE--%$(bindir)/agents.e%" \
	    -e "s%--AGENTSOBJ--%$(bindir)/agents.o%" \
	    -e "s%--COMPILERDIR--%$(compilerdir)%" \
	    -e "s%--EMULATORDIR--%$(emulatordir)%" \
	    -e "s%--ENVIRONMENTDIR--%$(environmentdir)%" \
	    -e "s%--VERSION--%$(datadir)/version.pam%" \
	    -e "s%--CC--%$(CC)%" \
	    -e "s%--CFLAGS--%$(ALL_CFLAGS)%" \
	    -e "s%--LDFLAGS--%$(LDFLAGS)%" \
	    -e "s%--AGENTSFLAGS--%$(SAGENTSFLAGS)%" \
	    agents.in >> agents.tm1
# The patterns sought by the following sed commands come from $(SAGENTSFLAGS)
# and from the configure variables CFLAGS_GMP and LDFLAGS_GMP via $(ALL_CFLAGS)
# and $(LD_FLAGS).  This tactic is clumsy but it works.
	sed -e "s%-I$(srcdir)/emulator%-I$(emulatordir)%" \
	    -e "s%-I$(gmpdir)%-I$(includedir)%" \
	    -e "s%-L$(gmpdir)%-L$(libdir)%" \
	    agents.tm1 > agents.tm2
	chmod +x agents.tm2
	$(INSTALL_PROGRAM) agents.tm2 $(bindir)/agents
	rm agents.tm1 agents.tm2

install-library:
	(cd library; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' install)

install-tools:
	(cd tools; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' install)

install-demos:
	(cd demos; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' install)

install-man:
	$(INSTALL_DATA) agents.1 $(mandir)/agents.$(manext)

install-info:
	(cd doc; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' install)

install-emacs-mode:
	$(INSTALL_DATA) akl.el $(datadir)
	$(INSTALL_DATA) comint.el $(datadir)


uninstall:	uninstall-gmp uninstall-emulator uninstall-compiler \
		uninstall-environment uninstall-version uninstall-agents \
		uninstall-library uninstall-tools uninstall-demos \
		uninstall-man uninstall-info

# Note the note about this target at ***** above.
uninstall-gmp:
	rm -f $(libdir)/libgmp.a \
	      $(includedir)/*.h

uninstall-emulator:
	rm -f $(bindir)/agents.o \
	      $(bindir)/agents.e \
	      $(emulatordir)/*.h

uninstall-compiler:
	(cd oldcompiler; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' uninstall)

uninstall-environment:
	(cd environment; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' uninstall)

uninstall-version:
	rm -f $(datadir)/version.pam

uninstall-agents:
	rm -f $(bindir)/agents

uninstall-library:
	(cd library; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' uninstall)

uninstall-tools:
	(cd tools; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' uninstall)

uninstall-demos:
	(cd demos; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' uninstall)

uninstall-man:
	rm -f $(mandir)/agents.$(manext)

uninstall-info:
	(cd doc; \
	 $(MAKE) $(MFLAGS) $(INSTALL_VARS_TO_PASS) \
		 INSTALL_VARS_TO_PASS='$(INSTALL_VARS_TO_PASS)' uninstall)


clean:
	(cd gmp; \
	 $(MAKE) $(MFLAGS) clean)
	(cd emulator; \
	 $(MAKE) $(MFLAGS) clean; \
	 rm -f oagents.o oagents tcovagents.o tcovagents)
	(cd oldcompiler; \
	 $(MAKE) $(MFLAGS) clean)
	(cd environment; \
	 $(MAKE) $(MFLAGS) clean)
	rm -f version.pam agents
	(cd library; \
	 $(MAKE) $(MFLAGS) clean)
	(cd tools; \
	 $(MAKE) $(MFLAGS) clean)
	(cd demos; \
	 $(MAKE) $(MFLAGS) clean)
	(cd doc; \
	 $(MAKE) $(MFLAGS) clean)
	(cd tests; \
	 rm -f agents.log agents.sum unique_name)


mostlyclean:	clean


distclean:	clean
	rm -f Makefile config.status


realclean:	distclean
# Omitted pending restoration of configure to gmp.
#	(cd gmp; \
#	 $(MAKE) $(MFLAGS) realclean)
	(cd emulator; \
	 $(MAKE) $(MFLAGS) realclean)
	(cd oldcompiler; \
	 $(MAKE) $(MFLAGS) realclean)
	(cd environment; \
	 $(MAKE) $(MFLAGS) realclean)
	(cd library; \
	 $(MAKE) $(MFLAGS) realclean)
	(cd tools; \
	 $(MAKE) $(MFLAGS) realclean)
	(cd demos; \
	 $(MAKE) $(MFLAGS) realclean)
	(cd doc; \
	 $(MAKE) $(MFLAGS) realclean)
	(cd tests; \
	 rm -f agents.plog agents.psum)


TAGS:
	(cd emulator; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' TAGS)


info:
	(cd doc; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' info)


dvi:
	(cd doc; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' dvi)


ps:
	(cd doc; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' ps)


dist:
	echo "The dist target is not implemented yet."


check:
	(cd tests; \
	 $(RUNTEST) $(RUNTESTFLAGS) --tool agents AGENTS=../agents)


check-coverage:	make-tcovagents test-tcovagents tcov-tcovagents

make-tcovagents:
	(cd emulator; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' \
		 AGENTSFLAGS="$(OAGENTSFLAGS) -a" clean agents; \
	 mv agents.o tcovagents.o; \
	 mv agents tcovagents)

test-tcovagents:
	(cd tests; \
	 $(RUNTEST) $(RUNTESTFLAGS) --tool agents AGENTS="../agents -tcov")

tcov-tcovagents:
	(cd emulator; \
	 ls *.d | sed "s/\.d/\.c/" | xargs tcov; \
	 cat *.tcov > ../agents.tcov)
	echo "See agents.tcov for tcov-format profile."


eagents:
	(cd emulator; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' \
		 AGENTSFLAGS="$(EAGENTSFLAGS)" clean agents; \
	 mv agents.o eagents.o; \
	 mv agents eagents)

oagents:
	(cd emulator; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' \
		 AGENTSFLAGS="$(OAGENTSFLAGS)" clean agents; \
	 mv agents.o oagents.o; \
	 mv agents oagents)

tagents:
	(cd emulator; \
	 $(MAKE) $(MFLAGS) $(VARS_TO_PASS) VARS_TO_PASS='$(VARS_TO_PASS)' \
		 AGENTSFLAGS="$(TAGENTSFLAGS)" clean agents; \
	 mv agents.o tagents.o; \
	 mv agents tagents)
