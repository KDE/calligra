
INCLUDES  = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula2 -I$(interfacedir) $(all_includes)
KDE_CXXFLAGS = $(USE_RTTI)

lib_LTLIBRARIES = libkformulapart.la kformulamain.la
## The part
libkformulapart_la_SOURCES = kformula_doc.cc \
	kformula_view.cc kformula_factory.cc \
	matrixwidget.cc
libkformulapart_la_LDFLAGS = $(KDE_PLUGIN)
libkformulapart_la_LIBADD = ../lib/kofficeui/libkofficeui.la ../lib/kformula2/libkformula2.la
libkformulapart_la_METASOURCES = AUTO

## The kdeinit loadable module
kformulamain_la_SOURCES = main.cc
kformulamain_la_LDFLAGS = $(all_libraries) -module -avoid-version
kformulamain_la_LIBADD = libkformulapart.la

## The executable
bin_PROGRAMS = kformula
kformula_SOURCES = dummy.cc
kformula_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kformula_LDADD = kformulamain.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS =  kformula_doc.h \
		  kformula_view.h kformula_factory.h formuladef.h \
		  matrixwidget.h

kdelnk_DATA = kformula.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-kformula.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = kformula.rc
rcdir = $(kde_datadir)/kformula

SUBDIRS =  pics

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc -o $(podir)/kformula.pot
