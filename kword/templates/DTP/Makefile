# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.3 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = ../../..
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

DISTDIR =

pkgdatadir = $(datadir)/koffice
pkglibdir = $(libdir)/koffice
pkgincludedir = $(includedir)/koffice

top_builddir = ../../..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
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
GMSGFMT = /usr/bin/msgfmt
IDL = /usr/local/bin/idl
KDE_EXTRA_RPATH = 
KDE_INCLUDES = -I/usr/local/kde/include
KDE_LDFLAGS = -L/usr/local/kde/lib
KDE_RPATH = -rpath $(kde_libraries) -rpath $(qt_libraries) -rpath $(x_libraries)
KOFFICE_INCLUDES = -I /usr/src/stadl/koffice/lib/kofficeui -I /usr/src/stadl/koffice/lib/kofficecore -I /usr/src/stadl/koffice/lib/koml
KOFFICE_LIBS = /usr/src/stadl/koffice/lib/kofficeui/libkofficeui.la /usr/src/stadl/koffice/lib/kofficecore/libkofficecore.la /usr/src/stadl/koffice/lib/koml/libkoml.la
LD = /usr/lib/gcc-lib/alphaev56-unknown-linux-gnu/egcs-2.90.29/ld
LIBCOMPAT = 
LIBCRYPT = -lcrypt
LIBDL = -ldl
LIBJPEG = -ljpeg
LIBMICO = -lmico2.2.1 -ldl
LIBOBJS = @LIBOBJS@
LIBPTHREAD = -lpthread
LIBPYTHON = -lpython1.5
LIBQIMGIO = 
LIBSOCKET = 
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LIBUCB = 
LN_S = ln -s
MAKEINFO = makeinfo
MICO_INCLUDES = -I/usr/local/include
MICO_LDFLAGS = -L/usr/local/lib
MOC = /usr/lib/qt/bin/moc
MSGFMT = /usr/bin/msgfmt
NM = /usr/bin/nm -B
PACKAGE = koffice
PAMINC = @PAMINC@
PAMLIBPATHS = @PAMLIBPATHS@
PAMLIBS = @PAMLIBS@
PYTHONINC = -I/usr/local/include/python1.5
PYTHONLIB = -L/usr/local/lib/python1.5/config
QT_INCLUDES = -I/usr/lib/qt/include
QT_LDFLAGS = -L/usr/lib/qt/lib
RANLIB = ranlib
USE_NLS = yes
VERSION = 0.1
XGETTEXT = /usr/bin/xgettext
XPMINC = @XPMINC@
XPMLIB = @XPMLIB@
X_EXTRA_LIBS = 
X_INCLUDES = -I/usr/X11R6/include
X_LDFLAGS = -L/usr/X11R6/lib
all_includes = -I/usr/local/kde/include -I/usr/lib/qt/include 
all_libraries = -L/usr/local/kde/lib -L/usr/lib/qt/lib  -L/usr/X11R6/lib
idldir = $(includedir)/idl
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
qt_libraries = /usr/lib/qt/lib
x_includes = /usr/X11R6/include
x_libraries = /usr/X11R6/lib

templates_DATA = SimpleLayout.xpm SimpleLayout.kwt
templatesdir = $(kde_datadir)/kword/templates/DTP
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../../../config.h
CONFIG_CLEAN_FILES = 
DATA =  $(templates_DATA)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
all: Makefile $(DATA)

.SUFFIXES:
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && $(AUTOMAKE) --gnu kword/templates/DTP/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status $(BUILT_SOURCES)
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


install-templatesDATA: $(templates_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(templatesdir)
	@list='$(templates_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(templatesdir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(templatesdir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(templatesdir)/$$p"; \
	    $(INSTALL_DATA) $$p $(DESTDIR)$(templatesdir)/$$p; \
	  fi; fi; \
	done

uninstall-templatesDATA:
	@$(NORMAL_UNINSTALL)
	list='$(templates_DATA)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(templatesdir)/$$p; \
	done
tags: TAGS
TAGS:


distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = kword/templates/DTP

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	distdir=`cd $(distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu kword/templates/DTP/Makefile
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

install-data: install-templatesDATA
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

uninstall: uninstall-templatesDATA

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs:
	$(mkinstalldirs)  $(DATADIR)$(templatesdir)


mostlyclean-generic:
	-test -z "$(MOSTLYCLEANFILES)" || rm -f $(MOSTLYCLEANFILES)

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	-rm -f Makefile $(DISTCLEANFILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	-test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)
	-test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)
mostlyclean:  mostlyclean-generic

clean:  clean-generic mostlyclean

distclean:  distclean-generic clean
	-rm -f config.status
	-rm -f libtool

maintainer-clean:  maintainer-clean-generic distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: uninstall-templatesDATA install-templatesDATA tags distdir info \
dvi installcheck install-exec install-data install uninstall all \
installdirs mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
