
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
###### Problem: we can't have one, since "kformula" is the name of the lib already.
###### The only solution is to rename either the lib, or the binary.
###### I suggest "libkformulalib" once we break BC.
###### Meanwhile, having a kdeinit module with the wrong name is useless -> disabled.
#kdeinit_LTLIBRARIES = kformula.la
#kformula_la_SOURCES = main.cc
#kformula_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kformula_la_LIBADD = $(LIB_KOFFICECORE)

## The executable (remove this when enabling the kdeinit module above, leave only bin_PROGRAMS=<nothing>)
bin_PROGRAMS = kformula
kformula_SOURCES = main.cc
kformula_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kformula_LDADD = $(LIB_KOFFICECORE)

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
