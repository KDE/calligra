####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula $(all_includes)
#liblateximport_la_LDFLAGS	= -module -avoid-version -no-undefined
#liblateximport_la_LIBADD = $(KOFFICE_LIBS)
liblatexexport_la_LDFLAGS	= -module -avoid-version -no-undefined
liblatexexport_la_LIBADD    = $(KOFFICE_LIBS) $(top_builddir)/lib/kformula/libkformula.la

####### Files

kde_module_LTLIBRARIES = liblatexexport.la
#		liblateximport.la

#liblateximport_la_SOURCES = lateximport.cc lateximport_factory.cc
# elementt.cc listetextzone.cc listefootnote.cc listepara.cc 

liblatexexport_la_SOURCES = anchor.cc document.cc element.cc fileheader.cc footnote.cc \
format.cc formula.cc latexexport.cc latexexport_factory.cc latexexportdia.cc \
layout.cc listtable.cc para.cc pictureformat.cc \
picturezone.cc table.cc texte.cc textformat.cc textzone.cc \
xml2latexparser.cc xmlparser.cc

noinst_HEADERS = anchor.h border.h document.h element.h fileheader.h footnote.h \
format.h formula.h latexexport.h latexexport_factory.h latexexportdia.h \
layout.h listtable.h para.h pictureformat.h \
picturezone.h table.h texte.h textformat.h textzone.h \
xml2latexparser.h xmlparser.h  \
latexexportdia.h

######## Debug
check_PROGRAMS = texlauncher
texlauncher_SOURCES = texlauncher.cc
texlauncher_LDADD = liblatexexport.la
texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)


METASOURCES = AUTO

service_DATA = kword_latex_export.desktop
#kword_latex_import.desktop
servicedir = $(kde_servicesdir)
