# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.2 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

####### General stuff


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

pkgdatadir = $(datadir)/koffice
pkglibdir = $(libdir)/koffice
pkgincludedir = $(includedir)/koffice

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
build_alias = @build_alias@
build_triplet = @build@
host_alias = i686-pc-linux-gnulibc1
host_triplet = i686-pc-linux-gnulibc1
target_alias = @target_alias@
target_triplet = @target@
CC = gcc
CPP = gcc -E
CXX = g++
GLINC = @GLINC@
GLLIB = @GLLIB@
GMSGFMT = /usr/bin/msgfmt
IDL = /usr/local/bin/idl
KDE_EXTRA_RPATH = 
KDE_INCLUDES = -I/opt/kde/include
KDE_LDFLAGS = -L/opt/kde/lib
KDE_RPATH = -rpath $(kde_libraries) -rpath $(x_libraries)
KOFFICE_INCLUDES = -I /home/weis/kde/koffice2/lib/kofficeui -I /home/weis/kde/koffice2/lib/kofficecore -I /home/weis/kde/koffice2/lib/koml
KOFFICE_LIBS = /home/weis/kde/koffice2/lib/kofficeui/libkofficeui.la /home/weis/kde/koffice2/lib/kofficecore/libkofficecore.la /home/weis/kde/koffice2/lib/koml/libkoml.la
LD = /usr/bin/ld
LIBCOMPAT = 
LIBCRYPT = 
LIBDL = -ldl
LIBJPEG = -ljpeg
LIBMICO = -lmico2.2.1 -ldl
LIBOBJS = @LIBOBJS@
LIBPNG = @LIBPNG@
LIBPTHREAD = -lpthread
LIBPYTHON = -lpython1.5
LIBQIMGIO = 
LIBSOCKET = 
LIBTIFF = @LIBTIFF@
LIBTOOL = $(SHELL) $(top_builddir)/libtool --silent
LIBUCB = 
LIB_KDECORE = -lkdecore -lXext $(LIB_QT)
LIB_KDEUI = -lkdeui $(LIB_KDECORE)
LIB_KFILE = -lkfile $(LIB_KFM) $(LIB_KDEUI)
LIB_KFM = -lkfm $(LIB_KDECORE)
LIB_KHTMLW = @LIB_KHTMLW@
LIB_KIMGIO = @LIB_KIMGIO@
LIB_QT = -lqt $(LIB_X11)
LIB_X11 = -lX11 $(LIBSOCKET)
LN_S = ln -s
MAKEINFO = makeinfo
MICO_INCLUDES = -I/usr/local/include
MICO_LDFLAGS = -L/usr/local/lib
MOC = /usr/bin/moc
MSGFMT = /usr/bin/msgfmt
NM = /usr/bin/nm -B
PACKAGE = koffice
PAMINC = @PAMINC@
PAMLIBPATHS = @PAMLIBPATHS@
PAMLIBS = @PAMLIBS@
PYTHONINC = -I/usr/local/include/python1.5
PYTHONLIB = -L/usr/local/lib/python1.5/config
QT_INCLUDES = -I/usr/lib/qt/include
QT_LDFLAGS = 
RANLIB = ranlib
USE_NLS = yes
VERSION = 0.1
XGETTEXT = /usr/bin/xgettext
XPMINC = @XPMINC@
XPMLIB = @XPMLIB@
X_EXTRA_LIBS = 
X_INCLUDES = -I/usr/X11R6/include
X_LDFLAGS = -L/usr/X11R6/lib
all_includes = -I/opt/kde/include -I/usr/lib/qt/include  -I/usr/X11R6/include
all_libraries = -L/opt/kde/lib  -L/usr/X11R6/lib
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
qt_libraries = 
x_includes = /usr/X11R6/include
x_libraries = /usr/X11R6/lib

INCLUDES= -I../koml -I../store -I$(idldir) $(all_includes) $(MICO_INCLUDES)
LDFLAGS	= $(all_libraries)  -version-info 1:0:0
LIBADD  = -lkom -lkfile -lkdeui -lkdecore -lqt -lkdecore \
	$(LIBMICO) -lXext -lX11

####### Files

lib_LTLIBRARIES = libkofficecore.la
BUILT_SOURCES =  moc_koFrame.cc moc_koMainWindow.cc
libkofficecore_la_SOURCES = koffice.cc koView.cc koFrame.cc koIMR.cc koScanParts.cc koStream.cc \
			    koMediator.cc koDocument.cc koPrintExt.cc koMainWindow.cc $(BUILT_SOURCES)
include_HEADERS = koffice.h koView.h koFrame.h koIMR.h koScanParts.h koStream.h koDocument.h koPrintExt.h koFactory.h \
		  koMainWindow.h koMediator.h

idl_DATA = koffice.idl
idl_HEADERS =  koffice.h

SUFFICES = .h .moc
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../../config.h
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(lib_LTLIBRARIES)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I../..
CPPFLAGS = 
LIBS = 
libkofficecore_la_LDFLAGS = 
libkofficecore_la_LIBADD = 
libkofficecore_la_OBJECTS =  koffice.lo koView.lo koFrame.lo koIMR.lo \
koScanParts.lo koStream.lo koMediator.lo koDocument.lo koPrintExt.lo \
koMainWindow.lo moc_koFrame.lo moc_koMainWindow.lo
CXXFLAGS = -O2 -Wall
CXXCOMPILE = $(CXX) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
LTCXXCOMPILE = $(LIBTOOL) --mode=compile $(CXX) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
CXXLINK = $(LIBTOOL) --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@
DATA =  $(idl_DATA)

HEADERS =  $(idl_HEADERS) $(include_HEADERS)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
DEP_FILES =  .deps/koDocument.P .deps/koFrame.P .deps/koIMR.P \
.deps/koMainWindow.P .deps/koMediator.P .deps/koPrintExt.P \
.deps/koScanParts.P .deps/koStream.P .deps/koView.P .deps/koffice.P \
.deps/moc_koFrame.P .deps/moc_koMainWindow.P
CXXMKDEP = $(CXX) -M $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
SOURCES = $(libkofficecore_la_SOURCES)
OBJECTS = $(libkofficecore_la_OBJECTS)

default: all

.SUFFIXES:
.SUFFIXES: .c .cc .lo .o
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu lib/kofficecore/Makefile

Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status $(BUILT_SOURCES)
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-libLTLIBRARIES:

clean-libLTLIBRARIES:
	test -z "$(lib_LTLIBRARIES)" || rm -f $(lib_LTLIBRARIES)

distclean-libLTLIBRARIES:

maintainer-clean-libLTLIBRARIES:

install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(libdir)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "$(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(libdir)/$$p"; \
	    $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLTLIBRARIES:
	$(NORMAL_UNINSTALL)
	list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  $(LIBTOOL)  --mode=uninstall rm -f $(libdir)/$$p; \
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

libkofficecore.la: $(libkofficecore_la_OBJECTS) $(libkofficecore_la_DEPENDENCIES)
	$(CXXLINK) -rpath $(libdir) $(libkofficecore_la_LDFLAGS) $(libkofficecore_la_OBJECTS) $(libkofficecore_la_LIBADD) $(LIBS)
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

install-includeHEADERS: $(include_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(includedir)
	@list='$(include_HEADERS)'; for p in $$list; do \
	  echo " $(INSTALL_DATA) $(srcdir)/$$p $(includedir)/$$p"; \
	  $(INSTALL_DATA) $(srcdir)/$$p $(includedir)/$$p; \
	done

uninstall-includeHEADERS:
	$(NORMAL_UNINSTALL)
	list='$(include_HEADERS)'; for p in $$list; do \
	  rm -f $(includedir)/$$p; \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES)
	here=`pwd` && cd $(srcdir) && mkid -f$$here/ID $(SOURCES) $(HEADERS)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES)
	tags=; \
	here=`pwd`; \
	test -z "$(ETAGS_ARGS)$(SOURCES)$(HEADERS)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $(SOURCES) $(HEADERS) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = lib/kofficecore

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu lib/kofficecore/Makefile
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  test -f $(distdir)/$$file \
	  || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	  || cp -p $$d/$$file $(distdir)/$$file; \
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
info:
dvi:
check: all
	$(MAKE)
installcheck:
install-exec: install-libLTLIBRARIES
	@$(NORMAL_INSTALL)

install-data: install-idlDATA install-idlHEADERS install-includeHEADERS
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

uninstall: uninstall-libLTLIBRARIES uninstall-idlDATA uninstall-idlHEADERS uninstall-includeHEADERS

all: Makefile $(LTLIBRARIES) $(DATA) $(HEADERS)

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs:
	$(mkinstalldirs)  $(libdir) $(idldir) $(idldir) $(includedir)


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
mostlyclean:  mostlyclean-libLTLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-tags mostlyclean-depend \
		mostlyclean-generic

clean:  clean-libLTLIBRARIES clean-compile clean-libtool clean-tags \
		clean-depend clean-generic mostlyclean

distclean:  distclean-libLTLIBRARIES distclean-compile distclean-libtool \
		distclean-tags distclean-depend distclean-generic clean
	rm -f config.status
	rm -f libtool

maintainer-clean:  maintainer-clean-libLTLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-tags maintainer-clean-depend \
		maintainer-clean-generic distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: default mostlyclean-libLTLIBRARIES distclean-libLTLIBRARIES \
clean-libLTLIBRARIES maintainer-clean-libLTLIBRARIES \
uninstall-libLTLIBRARIES install-libLTLIBRARIES mostlyclean-compile \
distclean-compile clean-compile maintainer-clean-compile \
mostlyclean-libtool distclean-libtool clean-libtool \
maintainer-clean-libtool uninstall-idlDATA install-idlDATA \
uninstall-idlHEADERS install-idlHEADERS uninstall-includeHEADERS \
install-includeHEADERS tags mostlyclean-tags distclean-tags clean-tags \
maintainer-clean-tags distdir mostlyclean-depend distclean-depend \
clean-depend maintainer-clean-depend info dvi installcheck install-exec \
install-data install uninstall all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


.h.moc:
	$(MOC) $< -o $@      

moc_koFrame.cc: koFrame.h
	$(MOC) koFrame.h -o moc_koFrame.cc

moc_koMainWindow.cc: koMainWindow.h
	$(MOC) koMainWindow.h -o moc_koMainWindow.cc

koDocument.h: koffice.h
koffice.h: koffice.idl
	$(IDL) --query-server-for-narrow --relative-paths -I$(idldir) koffice.idl

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
