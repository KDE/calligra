# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.2 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.


SHELL = /bin/sh

srcdir = .
top_srcdir = ../..
prefix = /opt/kde
exec_prefix = ${prefix}

bindir = /opt/kde/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

pkgdatadir = $(datadir)/killustrator
pkglibdir = $(libdir)/killustrator
pkgincludedir = $(includedir)/killustrator

top_builddir = ../..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = true
PRE_INSTALL = true
POST_INSTALL = true
NORMAL_UNINSTALL = true
PRE_UNINSTALL = true
POST_UNINSTALL = true
build_alias = i586-pc-linux-gnu
build_triplet = i586-pc-linux-gnu
host_alias = i586-pc-linux-gnu
host_triplet = i586-pc-linux-gnu
target_alias = i586-pc-linux-gnu
target_triplet = i586-pc-linux-gnu
CC = gcc
CPP = gcc -E
CXX = g++
GLINC = @GLINC@
GLLIB = @GLLIB@
GMSGFMT = /usr/local/bin/msgfmt
IDL = /usr/local/bin/idl
KDE_EXTRA_RPATH = 
KDE_INCLUDES = -I/opt/kde/include
KDE_LDFLAGS = -L/opt/kde/lib
KDE_RPATH = -rpath $(kde_libraries) -rpath $(qt_libraries) -rpath $(x_libraries)
LD = /usr/lib/gcc-lib/i586-pc-linux-gnu/egcs-2.90.21/ld
LIBCOMPAT = 
LIBCRYPT = -lcrypt
LIBDL = -ldl
LIBJPEG = -ljpeg
LIBMICO = -lmico2.0.8 -ldl
LIBOBJS = @LIBOBJS@
LIBPYTHON = @LIBPYTHON@
LIBQIMGIO = 
LIBSOCKET = 
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LIBUCB = 
LN_S = ln -s
MAKEINFO = makeinfo
MICO_INCLUDES = -I/usr/local/include
MICO_LDFLAGS = -L/usr/local/lib
MOC = /usr/bin/moc
MSGFMT = /usr/local/bin/msgfmt
NM = /usr/bin/nm -B
PACKAGE = killustrator
PAMINC = @PAMINC@
PAMLIBPATHS = @PAMLIBPATHS@
PAMLIBS = @PAMLIBS@
PYTHONINC = @PYTHONINC@
PYTHONLIB = @PYTHONLIB@
QT_INCLUDES = -I/usr/lib/qt/include
QT_LDFLAGS = -L/usr/lib/qt/lib
RANLIB = ranlib
USE_NLS = yes
VERSION = 0.3
XGETTEXT = /usr/local/bin/xgettext
XPMINC = @XPMINC@
XPMLIB = @XPMLIB@
X_EXTRA_LIBS = 
X_INCLUDES = -I/usr/X11R6/include
X_LDFLAGS = -L/usr/X11R6/lib
all_includes = -I/opt/kde/include -I/usr/lib/qt/include -I/usr/X11R6/include
all_libraries = -L/opt/kde/lib -L/usr/lib/qt/lib -L/usr/X11R6/lib
idldir = $(includedir)/idl
install_root = 
kde_appsdir = /opt/kde/share/applnk
kde_bindir = /opt/kde/bin
kde_cgidir = /opt/kde/cgi-bin
kde_confdir = /opt/kde/share/config
kde_datadir = /opt/kde/share/apps
kde_htmldir = /opt/kde/share/doc/HTML
kde_icondir = /opt/kde/share/icons
kde_includes = /opt/kde/include
kde_libraries = /opt/kde/lib
kde_locale = /opt/kde/share/locale
kde_mimedir = /opt/kde/share/mimelnk
kde_minidir = /opt/kde/share/icons/mini
kde_partsdir = /opt/kde/parts
kde_sounddir = /opt/kde/share/sounds
kde_toolbardir = /opt/kde/share/toolbar
kde_wallpaperdir = /opt/kde/share/wallpapers
qt_includes = /usr/lib/qt/include
qt_libraries = /usr/lib/qt/lib
topdir = /home/kai/project/killustrator
x_includes = /usr/X11R6/include
x_libraries = /usr/X11R6/lib

INCLUDES  = $(MICO_INCLUDES) $(all_includes) -I$(idldir) -I.. -I../share
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
FILTER_LIBS = ../filter/libfilter.a
XML_LIBS = ../xmlutils/libxml.a
KSHARE_LIBS = ../share/libkillu.a
LDADD = $(KSHARE_LIBS) $(XML_LIBS) $(FILTER_LIBS) \
	-lkofficeui -lkofficecore -lkoml \
	 -lkfile -lkfm -lop -lkom -lqt -lkdeui -lkdecore \
	$(LIBMICO) -lXext -lX11 $(LIBSOCKET)

####### Files

bin_PROGRAMS = killustrator.bin

killustrator_bin_SOURCES = KIllustrator_main.cc \
	KIllustrator_doc.cc \
	KIllustrator_shell.cc \
	KIllustrator_view.cc \
	KIllustrator.cc 

noinst_HEADERS = KIllustrator_main.h \
	KIllustrator_doc.h \
	KIllustrator_shell.h \
	KIllustrator_view.h \
	KIllustrator.h

SRCMETA = KIllustrator_main.moc \
	KIllustrator_shell.moc \
	KIllustrator_doc.moc \
	KIllustrator_view.moc

idl_DATA = KIllustrator.idl
idl_HEADERS = KIllustrator.h

kdelnk_DATA = killustrator_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

SUBDIRS = 

####### Implicit rules

SUFFIXES = .h .moc
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../../config.h
CONFIG_CLEAN_FILES = 
PROGRAMS =  $(bin_PROGRAMS)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I../..
CPPFLAGS = 
LIBS = 
killustrator_bin_OBJECTS =  KIllustrator_main.o KIllustrator_doc.o \
KIllustrator_shell.o KIllustrator_view.o KIllustrator.o
killustrator_bin_LDADD = $(LDADD)
killustrator_bin_DEPENDENCIES =  ../share/libkillu.a \
../xmlutils/libxml.a ../filter/libfilter.a
killustrator_bin_LDFLAGS = 
CXXFLAGS = -O2 -Wall
CXXCOMPILE = $(CXX) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
LTCXXCOMPILE = $(LIBTOOL) --mode=compile $(CXX) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
CXXLINK = $(LIBTOOL) --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@
DATA =  $(idl_DATA) $(kdelnk_DATA)

HEADERS =  $(idl_HEADERS) $(noinst_HEADERS)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
DEP_FILES =  .deps/KIllustrator.P .deps/KIllustrator_doc.P \
.deps/KIllustrator_main.P .deps/KIllustrator_shell.P \
.deps/KIllustrator_view.P
CXXMKDEP = $(CXX) -M $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
SOURCES = $(killustrator_bin_SOURCES)
OBJECTS = $(killustrator_bin_OBJECTS)

default: all

.SUFFIXES:
.SUFFIXES: .c .cc .h .lo .moc .o
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu src/koffice/Makefile

Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status $(BUILT_SOURCES)
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-binPROGRAMS:

clean-binPROGRAMS:
	test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)

distclean-binPROGRAMS:

maintainer-clean-binPROGRAMS:

install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(bindir)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(bindir)/`echo $$p|sed '$(transform)'`"; \
	    $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(bindir)/`echo $$p|sed '$(transform)'`; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	$(NORMAL_UNINSTALL)
	list='$(bin_PROGRAMS)'; for p in $$list; do \
	  rm -f $(bindir)/`echo $$p|sed '$(transform)'`; \
	done

.c.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	rm -f *.o core

clean-compile:

distclean-compile:
	rm -f *.tab.c

maintainer-clean-compile:

.c.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

mostlyclean-libtool:
	rm -f *.lo

clean-libtool:
	rm -rf .libs

distclean-libtool:

maintainer-clean-libtool:

killustrator.bin: $(killustrator_bin_OBJECTS) $(killustrator_bin_DEPENDENCIES)
	@rm -f killustrator.bin
	$(CXXLINK) $(killustrator_bin_LDFLAGS) $(killustrator_bin_OBJECTS) $(killustrator_bin_LDADD) $(LIBS)
.cc.o:
	$(CXXCOMPILE) -c $<
.cc.lo:
	$(LTCXXCOMPILE) -c $<

install-idlDATA: $(idl_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(idldir)
	@list='$(idl_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(idldir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(idldir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(idldir)/$$p"; \
	    $(INSTALL_DATA) $$p $(idldir)/$$p; \
	  fi; fi; \
	done

uninstall-idlDATA:
	$(NORMAL_UNINSTALL)
	list='$(idl_DATA)'; for p in $$list; do \
	  rm -f $(idldir)/$$p; \
	done

install-kdelnkDATA: $(kdelnk_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(kdelnkdir)
	@list='$(kdelnk_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(kdelnkdir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(kdelnkdir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(kdelnkdir)/$$p"; \
	    $(INSTALL_DATA) $$p $(kdelnkdir)/$$p; \
	  fi; fi; \
	done

uninstall-kdelnkDATA:
	$(NORMAL_UNINSTALL)
	list='$(kdelnk_DATA)'; for p in $$list; do \
	  rm -f $(kdelnkdir)/$$p; \
	done

install-idlHEADERS: $(idl_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(idldir)
	@list='$(idl_HEADERS)'; for p in $$list; do \
	  echo " $(INSTALL_DATA) $(srcdir)/$$p $(idldir)/$$p"; \
	  $(INSTALL_DATA) $(srcdir)/$$p $(idldir)/$$p; \
	done

uninstall-idlHEADERS:
	$(NORMAL_UNINSTALL)
	list='$(idl_HEADERS)'; for p in $$list; do \
	  rm -f $(idldir)/$$p; \
	done

# This directory's subdirectories are mostly independent; you can cd
# into them and run `make' without going through this Makefile.
# To change the values of `make' variables: instead of editing Makefiles,
# (1) if the variable is set in `config.status', edit `config.status'
#     (which will cause the Makefiles to be regenerated when you run `make');
# (2) otherwise, pass the desired values on the `make' command line.



all-recursive install-data-recursive install-exec-recursive \
installdirs-recursive install-recursive uninstall-recursive  \
check-recursive installcheck-recursive info-recursive dvi-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	for subdir in $(SUBDIRS); do \
	  target=`echo $@ | sed s/-recursive//`; \
	  echo "Making $$target in $$subdir"; \
	  (cd $$subdir && $(MAKE) $$target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"

mostlyclean-recursive clean-recursive distclean-recursive \
maintainer-clean-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	rev=''; for subdir in $(SUBDIRS); do rev="$$rev $$subdir"; done; \
	for subdir in $$rev; do \
	  target=`echo $@ | sed s/-recursive//`; \
	  echo "Making $$target in $$subdir"; \
	  (cd $$subdir && $(MAKE) $$target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"
tags-recursive:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  (cd $$subdir && $(MAKE) tags); \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES)
	here=`pwd` && cd $(srcdir) && mkid -f$$here/ID $(SOURCES) $(HEADERS)

TAGS: tags-recursive $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES)
	tags=; \
	here=`pwd`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test -f $$subdir/TAGS && tags="$$tags -i $$here/$$subdir/TAGS"; \
	done; \
	test -z "$(ETAGS_ARGS)$(SOURCES)$(HEADERS)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $(SOURCES) $(HEADERS) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = src/koffice

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu src/koffice/Makefile
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  test -f $(distdir)/$$file \
	  || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	  || cp -p $$d/$$file $(distdir)/$$file; \
	done
	for subdir in $(SUBDIRS); do		\
	  test -d $(distdir)/$$subdir		\
	  || mkdir $(distdir)/$$subdir		\
	  || exit 1;				\
	  chmod 777 $(distdir)/$$subdir;	\
	  (cd $$subdir && $(MAKE) top_distdir=../$(top_distdir) distdir=../$(distdir)/$$subdir distdir) \
	    || exit 1; \
	done

MKDEP = gcc -M $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)
-include .deps/.P
.deps/.P: $(BUILT_SOURCES)
	echo > $@

-include $(DEP_FILES)

mostlyclean-depend:

clean-depend:

distclean-depend:

maintainer-clean-depend:
	rm -rf .deps

.deps/%.P: %.c
	@echo "Computing dependencies for $<..."
	@o='o'; \
	test -n "$o" && o='$$o'; \
	$(MKDEP) $< >$@.tmp \
	  && sed "s,^\(.*\)\.o:,\1.$$o \1.l$$o $@:," < $@.tmp > $@ \
	  && rm -f $@.tmp

.deps/%.P: %.cc
	@echo "Computing dependencies for $<..."
	@o='o'; \
	$(CXXMKDEP) $< >$@.tmp \
	  && sed "s,^\(.*\)\.o:,\1.$$o \1.l$$o $@:," < $@.tmp > $@ \
	  && rm -f $@.tmp
info: info-recursive
dvi: dvi-recursive
check: all-am
	$(MAKE) check-recursive
installcheck: installcheck-recursive
all-am: Makefile $(PROGRAMS) $(DATA) $(HEADERS)

install-exec-am: install-binPROGRAMS install-exec-local

install-data-am: install-idlDATA install-kdelnkDATA install-idlHEADERS

uninstall-am: uninstall-binPROGRAMS uninstall-idlDATA uninstall-kdelnkDATA uninstall-idlHEADERS

install-exec: install-exec-recursive install-exec-am
	@$(NORMAL_INSTALL)

install-data: install-data-recursive install-data-am
	@$(NORMAL_INSTALL)

install: install-recursive install-exec-am install-data-am
	@:

uninstall: uninstall-recursive uninstall-am

all: all-recursive all-am

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs: installdirs-recursive
	$(mkinstalldirs)  $(bindir) $(idldir) $(kdelnkdir) $(idldir)


mostlyclean-generic:
	test -z "$(MOSTLYCLEANFILES)" || rm -f $(MOSTLYCLEANFILES)

clean-generic:
	test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	rm -f Makefile $(DISTCLEANFILES)
	rm -f config.cache config.log stamp-h stamp-h[0-9]*
	test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)
	test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)
mostlyclean-am:  mostlyclean-binPROGRAMS mostlyclean-compile \
		mostlyclean-libtool mostlyclean-tags mostlyclean-depend \
		mostlyclean-generic

clean-am:  clean-binPROGRAMS clean-compile clean-libtool clean-tags \
		clean-depend clean-generic mostlyclean-am

distclean-am:  distclean-binPROGRAMS distclean-compile distclean-libtool \
		distclean-tags distclean-depend distclean-generic \
		clean-am

maintainer-clean-am:  maintainer-clean-binPROGRAMS \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-tags maintainer-clean-depend \
		maintainer-clean-generic distclean-am

mostlyclean:  mostlyclean-recursive mostlyclean-am

clean:  clean-recursive clean-am

distclean:  distclean-recursive distclean-am
	rm -f config.status
	rm -f libtool

maintainer-clean:  maintainer-clean-recursive maintainer-clean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: default mostlyclean-binPROGRAMS distclean-binPROGRAMS \
clean-binPROGRAMS maintainer-clean-binPROGRAMS uninstall-binPROGRAMS \
install-binPROGRAMS mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile mostlyclean-libtool distclean-libtool \
clean-libtool maintainer-clean-libtool uninstall-idlDATA \
install-idlDATA uninstall-kdelnkDATA install-kdelnkDATA \
uninstall-idlHEADERS install-idlHEADERS install-data-recursive \
uninstall-data-recursive install-exec-recursive \
uninstall-exec-recursive installdirs-recursive uninstalldirs-recursive \
all-recursive check-recursive installcheck-recursive info-recursive \
dvi-recursive mostlyclean-recursive distclean-recursive clean-recursive \
maintainer-clean-recursive tags tags-recursive mostlyclean-tags \
distclean-tags clean-tags maintainer-clean-tags distdir \
mostlyclean-depend distclean-depend clean-depend \
maintainer-clean-depend info dvi installcheck all-am install-exec-am \
install-data-am uninstall-am install-exec install-data install \
uninstall all installdirs mostlyclean-generic distclean-generic \
clean-generic maintainer-clean-generic clean mostlyclean distclean \
maintainer-clean


.h.moc: 
	$(MOC) $< -o $@

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) killustrator $(bindir)/killustrator

KIllustrator.h: KIllustrator.idl
	$(IDL) --query-server-for-narrow -I$(idldir) KIllustrator.idl

KIllustrator.cc: KIllustrator.h
KIllustrator_main.cc: KIllustrator.h KIllustrator_main.moc
KIllustrator_doc.cc: KIllustrator.h KIllustrator_doc.moc
KIllustrator_shell.cc: KIllustrator.h KIllustrator_shell.moc
KIllustrator_view.cc: KIllustrator.h KIllustrator_view.moc

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
