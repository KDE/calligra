
INCLUDES  = $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(KOTEXT_INCLUDES) -I$(interfacedir) $(all_includes)
KDE_CXXFLAGS = $(USE_RTTI)

## The part
kde_module_LTLIBRARIES = libkformulapart.la
libkformulapart_la_SOURCES = kformula_doc.cc \
	kformula_view.cc kformula_factory.cc kformulawidget.cc kfconfig.cc \
	kformula_view_iface.skel kformula_view_iface.cc formulastring.cc fsparser.cc
libkformulapart_la_LDFLAGS = $(KDE_PLUGIN)
libkformulapart_la_LIBADD = $(LIB_KOFFICEUI) $(LIB_KFORMULA) $(LIB_KOTEXT)
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
		  kformula_view.h kformula_factory.h \
		  kformula_aboutdata.h kformula_view_iface.h formulastring.h fsparser.h

kdelnk_DATA = kformula.desktop
kdelnkdir = $(kde_appsdir)/Office

rc_DATA = kformula.rc kformula_readonly.rc
rcdir = $(kde_datadir)/kformula

SUBDIRS =  pics

tipdir = $(kde_datadir)/kformula
tip_DATA = tips

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc kformula_aboutdata.h -o $(podir)/kformula.pot
kde_services_DATA = kformulapart.desktop
