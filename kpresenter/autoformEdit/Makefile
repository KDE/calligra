# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.2 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

####### General stuff


SHELL = /bin/sh

srcdir = .
top_srcdir = ../..
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

INCLUDES = $(all_includes) -I../../lib/kofficeui
LDFLAGS =  $(all_libraries)
LDADD =  ../../lib/kofficeui/libkofficeui.la \
	../../lib/kofficecore/libkofficecore.la ../../lib/koml/libkoml.la \
	-lkdecore -lkdeui -lqt -lm -lXext -lX11 -lkofficeui -lop -lkom \
	-lkfile -lkfm -lkoml

bin_PROGRAMS = kautoformEdit

####### Files

kautoformEdit_SOURCES = main.cc autoformEdit.cc editWidget.cc \
	drawWidget.cc atfinterpreter.cc pntInsDia.cc afchoose.cc \
	saveAsDia.cc

noinst_HEADERS	= autoformEdit.h editWidget.h drawWidget.h atfinterpreter.h\
		pntInsDia.h afchoose.h saveAsDia.h 

SRCMETA = autoformEdit.moc editWidget.moc drawWidget.moc atfinterpreter.moc\
	pntInsDia.moc afchoose.moc saveAsDia.moc
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../../config.h
CONFIG_CLEAN_FILES = 
PROGRAMS =  $(bin_PROGRAMS)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I../..
CPPFLAGS = 
LIBS = 
kautoformEdit_OBJECTS =  main.o autoformEdit.o editWidget.o drawWidget.o \
atfinterpreter.o pntInsDia.o afchoose.o saveAsDia.o
kautoformEdit_LDADD = $(LDADD)
kautoformEdit_DEPENDENCIES =  ../../lib/kofficeui/libkofficeui.la \
../../lib/kofficecore/libkofficecore.la ../../lib/koml/libkoml.la
kautoformEdit_LDFLAGS = 
CXXFLAGS = -O2 -Wall
CXXCOMPILE = $(CXX) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
LTCXXCOMPILE = $(LIBTOOL) --mode=compile $(CXX) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
CXXLINK = $(LIBTOOL) --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@
HEADERS =  $(noinst_HEADERS)

DIST_COMMON =  ChangeLog Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
DEP_FILES =  .deps/afchoose.P .deps/atfinterpreter.P \
.deps/autoformEdit.P .deps/drawWidget.P .deps/editWidget.P .deps/main.P \
.deps/pntInsDia.P .deps/saveAsDia.P
CXXMKDEP = $(CXX) -M $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
SOURCES = $(kautoformEdit_SOURCES)
OBJECTS = $(kautoformEdit_OBJECTS)

default: all

.SUFFIXES:
.SUFFIXES: .c .cc .lo .o
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu kpresenter/autoformEdit/Makefile

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

kautoformEdit: $(kautoformEdit_OBJECTS) $(kautoformEdit_DEPENDENCIES)
	@rm -f kautoformEdit
	$(CXXLINK) $(kautoformEdit_LDFLAGS) $(kautoformEdit_OBJECTS) $(kautoformEdit_LDADD) $(LIBS)
.cc.o:
	$(CXXCOMPILE) -c $<
.cc.lo:
	$(LTCXXCOMPILE) -c $<

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

subdir = kpresenter/autoformEdit

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu kpresenter/autoformEdit/Makefile
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
install-exec: install-binPROGRAMS
	@$(NORMAL_INSTALL)

install-data: 
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

uninstall: uninstall-binPROGRAMS

all: Makefile $(PROGRAMS) $(HEADERS)

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs:
	$(mkinstalldirs)  $(bindir)


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
mostlyclean:  mostlyclean-binPROGRAMS mostlyclean-compile \
		mostlyclean-libtool mostlyclean-tags mostlyclean-depend \
		mostlyclean-generic

clean:  clean-binPROGRAMS clean-compile clean-libtool clean-tags \
		clean-depend clean-generic mostlyclean

distclean:  distclean-binPROGRAMS distclean-compile distclean-libtool \
		distclean-tags distclean-depend distclean-generic clean
	rm -f config.status
	rm -f libtool

maintainer-clean:  maintainer-clean-binPROGRAMS maintainer-clean-compile \
		maintainer-clean-libtool maintainer-clean-tags \
		maintainer-clean-depend maintainer-clean-generic \
		distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: default mostlyclean-binPROGRAMS distclean-binPROGRAMS \
clean-binPROGRAMS maintainer-clean-binPROGRAMS uninstall-binPROGRAMS \
install-binPROGRAMS mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile mostlyclean-libtool distclean-libtool \
clean-libtool maintainer-clean-libtool tags mostlyclean-tags \
distclean-tags clean-tags maintainer-clean-tags distdir \
mostlyclean-depend distclean-depend clean-depend \
maintainer-clean-depend info dvi installcheck install-exec install-data \
install uninstall all installdirs mostlyclean-generic distclean-generic \
clean-generic maintainer-clean-generic clean mostlyclean distclean \
maintainer-clean


####### Build rules

all:  $(TARGET)

######## Meta objects 

autoformEdit.moc: autoformEdit.h
	$(MOC) autoformEdit.h -o autoformEdit.moc

editWidget.moc: editWidget.h
	$(MOC) editWidget.h -o editWidget.moc

drawWidget.moc: drawWidget.h
	$(MOC) drawWidget.h -o drawWidget.moc

atfinterpreter.moc: atfinterpreter.h
	$(MOC) atfinterpreter.h -o atfinterpreter.moc

pntInsDia.moc: pntInsDia.h
	$(MOC) pntInsDia.h -o pntInsDia.moc

ainfodia.moc: ainfodia.h
	$(MOC) ainfodia.h -o ainfodia.moc

afchoose.moc: afchoose.h
	$(MOC) afchoose.h -o afchoose.moc

saveAsDia.moc: saveAsDia.h
	$(MOC) saveAsDia.h -o saveAsDia.moc

afchoose.cc: afchoose.moc
atfinterpreter.cc: atfinterpreter.moc
backdia.cc: backdia.moc
graphobj.cc: graphobj.moc
kcharselect.cc: kcharselect.moc
kcharselectdia.cc: kcharselectdia.moc
kenumlistdia.cc: kenumlistdia.moc
kpresenter_doc.cc: kpresenter_doc.moc
kpresenter_main.cc: kpresenter_main.moc
kpresenter_shell.cc: kpresenter_shell.moc
kpresenter_view.cc: kpresenter_view.moc
ktextobject.cc: ktextobject.moc
page.cc: page.moc
styledia.cc: styledia.moc      

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
