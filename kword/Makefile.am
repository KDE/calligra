
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula -I$(top_srcdir)/qt3stuff $(all_includes)

####### Files

bin_PROGRAMS = kword
lib_LTLIBRARIES = libkwordpart.la kword.la

libkwordpart_la_SOURCES = kwfactory.cc kwdoc.cc kwview.cc kwcanvas.cc \
                kwtextparag.cc kwstyle.cc \
		paragdia.cc searchdia.cc \
                imagecollection.cc image.cc \
		kwframe.cc kwtextframeset.cc kwgroupmanager.cc \
		framedia.cc kcharselectdia.cc kwutils.cc preview.cc \
                tabledia.cc insdia.cc deldia.cc docstruct.cc \
                clipbrd_dnd.cc kwconfig.cc \
                contents.cc  stylist.cc \
                border.cc resizehandles.cc kwcommand.cc

libkwordpart_la_LDFLAGS = $(KDE_LDFLAGS) -avoid-version -module -no-undefined
libkwordpart_la_LIBADD = ../lib/kofficeui/libkofficeui.la ../lib/kformula/libkformula.la ../qt3stuff/libqt3stuff.la -lkspell
libkwordpart_la_METASOURCES = AUTO

kword_la_SOURCES = main.cc
kword_la_LDFLAGS = $(all_libraries) -avoid-version -module
kword_la_LIBADD = libkwordpart.la

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
