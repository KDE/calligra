####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#liblateximport_la_LDFLAGS	= -module -avoid-version -no-undefined
#liblateximport_la_LIBADD = $(KOFFICE_LIBS)
liblatexexport_la_LDFLAGS	= -module -avoid-version -no-undefined
liblatexexportdia_la_LDFLAGS    =  -module -avoid-version -no-undefined
liblatexexport_la_LIBADD    = $(KOFFICE_LIBS)
liblatexexportdia_la_LIBADD = $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = liblatexexport.la liblatexexportdia.la
#		liblateximport.la

#liblateximport_la_SOURCES = lateximport.cc lateximport_factory.cc
# elementt.cc listetextzone.cc listefootnote.cc listepara.cc 

liblatexexport_la_SOURCES = document.cc element.cc fileheader.cc footnote.cc \
format.cc formula.cc latexexport.cc latexexport_factory.cc layout.cc \
listtable.cc para.cc pictureformat.cc \
picturezone.cc table.cc texte.cc textformat.cc textzone.cc \
xml2latexparser.cc xmlparser.cc

liblatexexportdia_la_SOURCES = latexexportdia.cc latexexportdia_factory.cc

# listelement.cc listeformat.cc 
# elementt.h listetextzone.h listefootnote.h listepara.h 

noinst_HEADERS = border.h document.h element.h fileheader.h footnote.h \
format.h formula.h latexexport.h latexexport_factory.h layout.h \
listtable.h para.h pictureformat.h \
picturezone.h table.h texte.h textformat.h textzone.h \
xml2latexparser.h xmlparser.h  \
latexexportdia.h latexexportdia_factory.h

#listelement.h listeformat.h 
#lateximport.h lateximport_factory.h

######## Debug
check_PROGRAMS = texlauncher
texlauncher_SOURCES = texlauncher.cc
texlauncher_LDADD = liblatexexport.la
texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)


METASOURCES = AUTO

service_DATA = kword_latex_export.desktop kword_latex_export_dia.desktop
#kword_latex_import.desktop
servicedir = $(kde_servicesdir)
