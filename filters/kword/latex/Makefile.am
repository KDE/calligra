####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#liblateximport_la_LDFLAGS	= -module -avoid-version -no-undefined
#liblateximport_la_LIBADD = $(KOFFICE_LIBS)
liblatexexport_la_LDFLAGS	= -module -avoid-version -no-undefined
liblatexexport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = liblatexexport.la
#		liblateximport.la

#liblateximport_la_SOURCES = lateximport.cc lateximport_factory.cc
# elementt.cc listetextzone.cc listefootnote.cc 

liblatexexport_la_SOURCES = document.cc element.cc fileheader.cc footnote.cc \
format.cc latexexport.cc latexexport_factory.cc layout.cc \
listelement.cc listeformat.cc listepara.cc para.cc pictureformat.cc \
picturezone.cc texte.cc textformat.cc textzone.cc \
xml2latexparser.cc xmlparser.cc xmlparse.cc

# elementt.h listetextzone.h listefootnote.h

noinst_HEADERS = document.h element.h fileheader.h footnote.h format.h \
latexexport.h latexexport_factory.h layout.h \
listelement.h listeformat.h listepara.h para.h pictureformat.h \
picturezone.h texte.h textformat.h textzone.h \
xml2latexparser.h xmlparser.h xmlparse.h

#lateximport.h lateximport_factory.h

METASOURCES = AUTO

service_DATA = kword_latex_export.desktop #kword_latex_import.desktop
servicedir = $(kde_servicesdir)
