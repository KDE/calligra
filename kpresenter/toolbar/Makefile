# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.2 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.


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

toolbar_DATA = 121.xpm alignCenter.xpm alignLeft.xpm alignRight.xpm \
	autoform.xpm bold.xpm circle.xpm clipart.xpm configure.xpm \
	delete.xpm effect.xpm enumList.xpm first.xpm italic.xpm \
	last.xpm line.xpm lined1.xpm lined2.xpm lineh.xpm linev.xpm \
	lower.xpm newPoint.xpm newfolder.xpm next.xpm normalText.xpm \
	parts.xpm pause.xpm pen.xpm picture.xpm prev.xpm raise.xpm \
	rectangle.xpm rectangle2.xpm rectangleRound.xpm rotate.xpm \
	screen.xpm start.xpm stop.xpm style.xpm text.xpm underl.xpm \
	unsortedList.xpm

toolbardir = $(kde_datadir)/kpresenter/toolbar
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../../config.h
CONFIG_CLEAN_FILES = 
DATA =  $(toolbar_DATA)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
default: all

.SUFFIXES:
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu kpresenter/toolbar/Makefile

Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status $(BUILT_SOURCES)
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


install-toolbarDATA: $(toolbar_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(toolbardir)
	@list='$(toolbar_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(toolbardir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(toolbardir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(toolbardir)/$$p"; \
	    $(INSTALL_DATA) $$p $(toolbardir)/$$p; \
	  fi; fi; \
	done

uninstall-toolbarDATA:
	$(NORMAL_UNINSTALL)
	list='$(toolbar_DATA)'; for p in $$list; do \
	  rm -f $(toolbardir)/$$p; \
	done
tags: TAGS
TAGS:


distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = kpresenter/toolbar

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu kpresenter/toolbar/Makefile
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  test -f $(distdir)/$$file \
	  || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	  || cp -p $$d/$$file $(distdir)/$$file; \
	done
info:
dvi:
check: all
	$(MAKE)
installcheck:
install-exec: 
	@$(NORMAL_INSTALL)

install-data: install-toolbarDATA
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

uninstall: uninstall-toolbarDATA

all: Makefile $(DATA)

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs:
	$(mkinstalldirs)  $(toolbardir)


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
mostlyclean:  mostlyclean-generic

clean:  clean-generic mostlyclean

distclean:  distclean-generic clean
	rm -f config.status
	rm -f libtool

maintainer-clean:  maintainer-clean-generic distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: default uninstall-toolbarDATA install-toolbarDATA tags distdir \
info dvi installcheck install-exec install-data install uninstall all \
installdirs mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
