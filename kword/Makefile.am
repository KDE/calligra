
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula -I$(top_srcdir)/qt3stuff $(all_includes)

lib_LTLIBRARIES = libkwordpart.la kword.la

## The part
## Note that the application links to it (for the aboutdata),
## so it can't be moved to kde_module (where normal parts are)
libkwordpart_la_SOURCES = kwfactory.cc kwdoc.cc kwview.cc kwcanvas.cc  \
                kwtextparag.cc kwstyle.cc \
		paragdia.cc searchdia.cc \
		kwframe.cc kwtextframeset.cc kwgroupmanager.cc \
		framedia.cc kcharselectdia.cc kwutils.cc preview.cc \
                tabledia.cc insdia.cc deldia.cc docstruct.cc \
                kwdrag.cc kwconfig.cc \
                contents.cc  stylist.cc \
                border.cc resizehandles.cc kwcommand.cc \
		autoformat.cc autoformatdia.cc

libkwordpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkwordpart_la_LIBADD = ../lib/kofficeui/libkofficeui.la ../lib/kformula/libkformula.la ../qt3stuff/libqt3stuff.la -lkspell
libkwordpart_la_METASOURCES = AUTO

## The kdeinit loadable module
kword_la_SOURCES = main.cc
kword_la_LDFLAGS = $(all_libraries) -avoid-version -module
kword_la_LIBADD = libkwordpart.la

## The executable
bin_PROGRAMS = kword
kword_SOURCES = dummy.cc
kword_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kword_LDADD = kword.la

dummy.cc:
	echo > dummy.cc

kdelnk_DATA = kword.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-kword.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kword
rc_DATA = kword.rc

SUBDIRS = . toolbar templates dtd pics

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc -o $(podir)/kword.pot
