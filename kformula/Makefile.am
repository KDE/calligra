
INCLUDES  = $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) -I$(interfacedir) $(all_includes)
KDE_CXXFLAGS = $(USE_RTTI)

## The part
kde_module_LTLIBRARIES = libkformulapart.la
libkformulapart_la_SOURCES = kformula_doc.cc \
	kformula_view.cc kformula_factory.cc kformulawidget.cc 
libkformulapart_la_LDFLAGS = $(KDE_PLUGIN)
libkformulapart_la_LIBADD = $(LIB_KOFFICEUI) $(LIB_KFORMULA)
libkformulapart_la_METASOURCES = AUTO

## The kdeinit loadable module
lib_LTLIBRARIES = kformulamain.la
kformulamain_la_SOURCES = main.cc
kformulamain_la_LDFLAGS = $(all_libraries) -module -avoid-version
kformulamain_la_LIBADD = $(LIB_KOFFICECORE)

## The executable
bin_PROGRAMS = kformula
kformula_SOURCES = dummy.cc
kformula_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kformula_LDADD = kformulamain.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS =  kformula_doc.h \
		  kformula_view.h kformula_factory.h formuladef.h \
		  matrixwidget.h kformula_aboutdata.h

kdelnk_DATA = kformula.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-kformula.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = kformula.rc
rcdir = $(kde_datadir)/kformula

SUBDIRS =  pics

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc kformula_aboutdata.h -o $(podir)/kformula.pot
