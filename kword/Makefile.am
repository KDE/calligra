
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula \
	-I$(top_srcdir)/qt3stuff $(all_includes)

## The part
kde_module_LTLIBRARIES = libkwordpart.la 
libkwordpart_la_SOURCES = kwfactory.cc kwdoc.cc kwview.cc kwcanvas.cc  \
                kwviewmode.cc \
		kwtextparag.cc kwstyle.cc kwformat.cc kwtextdocument.cc \
		kwframe.cc kwtextframeset.cc \
                kwtableframeset.cc kwtextimage.cc kwanchor.cc \
                kwimage.cc kwdrag.cc kwconfig.cc \
		paragdia.cc searchdia.cc \
		framedia.cc kcharselectdia.cc kwutils.cc preview.cc \
                tabledia.cc insdia.cc deldia.cc docstruct.cc \
                contents.cc counter.cc stylist.cc \
                border.cc resizehandles.cc kwcommand.cc \
		autoformat.cc autoformatdia.cc fontdia.cc \
		variable.cc variabledlgs.cc serialletter.cc \
		kwChangeCaseDia.cc kweditpersonnalexpressiondia.cc

libkwordpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkwordpart_la_LIBADD = ../lib/kofficeui/libkofficeui.la ../lib/kformula/libkformula.la ../qt3stuff/libqt3stuff.la -lkspell
libkwordpart_la_METASOURCES = AUTO

## The kdeinit loadable module
lib_LTLIBRARIES = kword.la
kword_la_SOURCES = main.cc
kword_la_LDFLAGS = $(all_libraries) -avoid-version -module
kword_la_LIBADD = ../lib/kofficecore/libkofficecore.la

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

SUBDIRS = . toolbar templates dtd pics expression

messages: rc.cpp
	perl ../kspread/extracti18n expression/*.xml > xml_doc.cc
	$(XGETTEXT) rc.cpp *.cc kwaboutdata.h -o $(podir)/kword.pot
	-rm xml_doc.cc
