
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula $(all_includes)

####### Files

bin_PROGRAMS = kword
lib_LTLIBRARIES = libkwordpart.la kword.la

libkwordpart_la_SOURCES = kword_doc.cc searchdia.cc char.cc kword_view.cc \
                fc.cc font.cc format.cc kword_page.cc parag.cc \
                paraglayout.cc formatcollection.cc paragdia.cc \
                imagecollection.cc image.cc kword_frame.cc framedia.cc \
                stylist.cc kcharselectdia.cc kword_utils.cc preview.cc \
                tabledia.cc insdia.cc deldia.cc variable.cc docstruct.cc \
                kword_undo.cc clipbrd_dnd.cc footnote.cc footnotedia.cc \
		autoformat.cc autoformatdia.cc variabledlgs.cc \
                serialletter.cc contents.cc kword_factory.cc \
                border.cc
libkwordpart_la_LDFLAGS = $(KDE_LDFLAGS) -avoid-version -module -no-undefined
libkwordpart_la_LIBADD = ../lib/kofficeui/libkofficeui.la ../lib/kformula/libkformula.la -lkspell
libkwordpart_la_METASOURCES = AUTO

kword_la_SOURCES = main.cc
kword_la_LDFLAGS = $(all_libraries) -avoid-version -module
kword_la_LIBADD = libkwordpart.la

kword_SOURCES = dummy.cc
kword_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kword_LDADD = kword.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS = char.h kword_doc.h kword_view.h \
		fc.h font.h format.h kword_page.h parag.h paraglayout.h \
		formatcollection.h paragdia.h imagecollection.h image.h \
		kword_frame.h framedia.h stylist.h  kcharselectdia.h \
		kword_utils.h searchdia.h preview.h tabledia.h insdia.h \
                deldia.h variable.h docstruct.h kword_undo.h clipbrd_dnd.h \
                footnote.h footnotedia.h autoformat.h autoformatdia.h \
                variabledlgs.h serialletter.h contents.h kword_factory.h \
                border.h

kdelnk_DATA = kword.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-kword.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kword
rc_DATA = kword.rc

SUBDIRS = . toolbar templates dtd pics

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc -o $(podir)/kword.pot
