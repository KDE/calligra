####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#liblateximport_la_LDFLAGS	= -module -avoid-version -no-undefined
#liblateximport_la_LIBADD = $(KOFFICE_LIBS)
liblatexexport_la_LDFLAGS	= -module -avoid-version -no-undefined
liblatexexport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

lib_LTLIBRARIES = liblatexexport.la
#		liblateximport.la

#liblateximport_la_SOURCES = lateximport.cc lateximport_factory.cc
liblatexexport_la_SOURCES = document.cc element.cc elementt.cc format.cc \
header.cc latexexport.cc latexexport_factory.cc layout.cc \
listelement.cc listepara.cc listetextzone.cc texte.cc textzone.cc \
xml2latexparser.cc xmlparser.cc xmlparse.cc

noinst_HEADERS = document.h element.h elementt.h format.h \
header.h latexexport.h latexexport_factory.h layout.h \
listelement.h listepara.h listetextzone.h texte.h textzone.h \
xml2latexparser.h xmlparser.h xmlparse.h

#lateximport.h lateximport_factory.h

METASOURCES = AUTO

data_DATA = x-latex.desktop
datadir = $(kde_mimedir)/text

service_DATA = kword_latex_import.desktop # kword_latex_export.desktop
servicedir = $(kde_servicesdir)
