####### General stuff

INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libhtmlimport_la_LDFLAGS = -module -avoid-version -no-undefined
libhtmlexport_la_LDFLAGS = -module -avoid-version -no-undefined
libhtmlexportdia_la_LDFLAGS = -module -avoid-version -no-undefined
libhtmlimport_la_LIBADD = $(KOFFICE_LIBS)
libhtmlexport_la_LIBADD = $(KOFFICE_LIBS)
libhtmlexportdia_la_LIBADD = $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = libhtmlimport.la libhtmlexport.la \
libhtmlexportdia.la

libhtmlimport_la_SOURCES = htmlimport.cc htmlimport_factory.cc\
htmlimportsax.cc ImportTags.cc ImportParser.cc\
ImportListener.cc ImportEntities.cc

libhtmlexport_la_SOURCES = htmlexport.cc htmlexport_factory.cc

libhtmlexportdia_la_SOURCES = htmlexportdia.cc htmlexportdia_factory.cc

noinst_HEADERS = htmlimport.h htmlimport_factory.h htmlimportsax.h\
htmlexport.h htmlexport_factory.h htmlexportdia.h htmlexportdia_factory.h\
ImportTags.h ImportParser.h ImportListener.h ImportEntities.h

METASOURCES = AUTO

service_DATA = kword_html_import.desktop kword_html_export.desktop kword_html_export_dia.desktop
servicedir = $(kde_servicesdir)
