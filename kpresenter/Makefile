# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.2 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.


SHELL = /bin/sh

srcdir = .
top_srcdir = ..
prefix = /usr/local/kde
exec_prefix = ${prefix}

bindir = /usr/local/kde/bin
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

top_builddir = ..

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
host_alias = alphaev56-unknown-linux-gnu
host_triplet = alphaev56-unknown-linux-gnu
target_alias = @target_alias@
target_triplet = @target@
CC = gcc
CPP = gcc -E
CXX = g++
GLINC = @GLINC@
GLLIB = @GLLIB@
GMSGFMT = @GMSGFMT@
KDE_EXTRA_RPATH = 
KDE_INCLUDES = -I/usr/local/kde/include
KDE_LDFLAGS = -L/usr/local/kde/lib
KDE_RPATH = -rpath $(kde_libraries) -rpath $(x_libraries)
LD = /usr/lib/gcc-lib/alphaev56-unknown-linux-gnu/egcs-2.90.27/ld
LIBCOMPAT = 
LIBCRYPT = -lcrypt
LIBDL = @LIBDL@
LIBJPEG = @LIBJPEG@
LIBMICO = -lmico2.0.5 -ldl
LIBOBJS = @LIBOBJS@
LIBPYTHON = -lpython1.5
LIBSOCKET = 
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LIBUCB = 
LN_S = ln -s
MAKEINFO = makeinfo
MOC = /usr/bin/moc
MSGFMT = @MSGFMT@
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
USE_NLS = @USE_NLS@
VERSION = 0.1
XGETTEXT = @XGETTEXT@
XPMINC = @XPMINC@
XPMLIB = @XPMLIB@
X_EXTRA_LIBS = 
X_INCLUDES = -I/usr/X11R6/include
X_LDFLAGS = -L/usr/X11R6/lib
all_includes = -I/usr/local/kde/include -I/usr/lib/qt/include -I/usr/X11R6/include
all_libraries = -L/usr/local/kde/lib -L/usr/X11R6/lib
idldir = $(prefix)/idl
install_root = 
kde_appsdir = /usr/local/kde/share/applnk
kde_bindir = /usr/local/kde/bin
kde_cgidir = /usr/local/kde/cgi-bin
kde_confdir = /usr/local/kde/share/config
kde_datadir = /usr/local/kde/share/apps
kde_htmldir = /usr/local/kde/share/doc/HTML
kde_icondir = /usr/local/kde/share/icons
kde_includes = /usr/local/kde/include
kde_libraries = /usr/local/kde/lib
kde_locale = /usr/local/kde/share/locale
kde_mimedir = /usr/local/kde/share/mimelnk
kde_minidir = /usr/local/kde/share/icons/mini
kde_partsdir = /usr/local/kde/parts
kde_sounddir = /usr/local/kde/share/sounds
kde_toolbardir = /usr/local/kde/share/toolbar
kde_wallpaperdir = /usr/local/kde/share/wallpapers
qt_includes = /usr/lib/qt/include
qt_libraries = 
x_includes = /usr/X11R6/include
x_libraries = /usr/X11R6/lib

INCLUDES=	-I../lib/kofficeui $(all_includes) -I$(idldir)
LDFLAGS	=	$(all_libraries) 
LDADD = ../lib/kofficeui/libkofficeui.la \
	../lib/kofficecore/libkofficecore.la ../lib/koml/libkoml.la \
	-lkdeui -lkdecore -lkfm $(LIBMICO) -lXext -lX11 -lkom -lop \
	-ljpeg -lkfile -lqt -lkoml

####### Files

bin_PROGRAMS = kpresenter
kpresenter_SOURCES = kpresenter_main.cc kpresenter_doc.cc kpresenter_view.cc \
	kpresenter.cc kpresenter_shell.cc graphobj.cc autoformEdit/atfinterpreter.cc\
	backdia.cc page.cc ktextobject.cc autoformEdit/afchoose.cc \
	jpeg.cc formats.cc qwmf.cc kenumlistdia.cc kcharselect.cc \
	kcharselectdia.cc \
	styledia.cc

noinst_HEADERS = kpresenter_main.h kpresenter_doc.h kpresenter_view.h \
		kpresenter.h kpresenter_shell.h graphobj.h autoformEdit/atfinterpreter.h \
		backdia.h page.h ktextobject.h autoformEdit/afchoose.h \
		styledia.h \
		jpeg.h formats.h qwmf.h global.h \
		kenumlistdia.h kcharselect.h kcharselectdia.h 

SRCMETA =	kpresenter_main.moc kpresenter_doc.moc kpresenter_view.moc \
		kpresenter_shell.moc graphobj.moc autoformEdit/atfinterpreter.moc \
		page.moc ktextobject.moc backdia.moc autoformEdit/afchoose.moc \
		styledia.moc \
		kenumlistdia.moc kcharselect.moc kcharselectdia.moc 

idl_DATA =	kpresenter.idl
idl_HEADER =    kpresenter.h

kdelnkdir = $(kde_datadir)/koffice/partlnk
kdelnk_DATA = kpresenter.kdelnk

SUBDIRS = autoformEdit autoforms pics toolbar 

####### Implicit rules

SUFFIXES = .h .moc 
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
PROGRAMS =  $(bin_PROGRAMS)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I..
CPPFLAGS = 
LIBS = 
kpresenter_OBJECTS =  kpresenter_main.o kpresenter_doc.o \
kpresenter_view.o kpresenter.o kpresenter_shell.o graphobj.o \
atfinterpreter.o backdia.o page.o ktextobject.o afchoose.o jpeg.o \
formats.o qwmf.o kenumlistdia.o kcharselect.o kcharselectdia.o \
styledia.o
kpresenter_LDADD = $(LDADD)
kpresenter_DEPENDENCIES =  ../lib/kofficeui/libkofficeui.la \
../lib/kofficecore/libkofficecore.la ../lib/koml/libkoml.la
kpresenter_LDFLAGS = 
CXXFLAGS = -O2 -Wall
CXXCOMPILE = $(CXX) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
LTCXXCOMPILE = $(LIBTOOL) --mode=compile $(CXX) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
CXXLINK = $(LIBTOOL) --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@
DATA =  $(idl_DATA) $(kdelnk_DATA)

HEADERS =  $(noinst_HEADERS)

DIST_COMMON =  ChangeLog INSTALL Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
DEP_FILES =  .deps/afchoose.P .deps/atfinterpreter.P .deps/backdia.P \
.deps/formats.P .deps/graphobj.P .deps/jpeg.P .deps/kcharselect.P \
.deps/kcharselectdia.P .deps/kenumlistdia.P .deps/kpresenter.P \
.deps/kpresenter_doc.P .deps/kpresenter_main.P .deps/kpresenter_shell.P \
.deps/kpresenter_view.P .deps/ktextobject.P .deps/page.P .deps/qwmf.P \
.deps/styledia.P
CXXMKDEP = $(CXX) -M $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
SOURCES = $(kpresenter_SOURCES)
OBJECTS = $(kpresenter_OBJECTS)

default: all

.SUFFIXES:
.SUFFIXES: .c .cc .h .lo .moc .o
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu kpresenter/Makefile

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

kpresenter: $(kpresenter_OBJECTS) $(kpresenter_DEPENDENCIES)
	@rm -f kpresenter
	$(CXXLINK) $(kpresenter_LDFLAGS) $(kpresenter_OBJECTS) $(kpresenter_LDADD) $(LIBS)
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

subdir = kpresenter

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu kpresenter/Makefile
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

install-exec-am: install-binPROGRAMS

install-data-am: install-idlDATA install-kdelnkDATA

uninstall-am: uninstall-binPROGRAMS uninstall-idlDATA uninstall-kdelnkDATA

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
	$(mkinstalldirs)  $(bindir) $(idldir) $(kdelnkdir)


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
install-data-recursive uninstall-data-recursive install-exec-recursive \
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

idl: kpresenter.idl
	idl -I$(idldir) kpresenter.idl

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
