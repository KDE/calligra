####### General stuff

#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)
INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexportold $(all_includes)

libasciiimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libasciiimport_la_LIBADD = $(KOFFICE_LIBS)

libasciiexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libasciiexport_la_LIBADD = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la
libasciiexport_la_LIBADD = $(KOFFICE_LIBS) ../libexportold/libkwordexportfiltersold.la

####### Files

kde_module_LTLIBRARIES = libasciiimport.la libasciiexport.la

libasciiimport_la_SOURCES = asciiimport.cc asciiimport_factory.cc

libasciiexport_la_SOURCES = asciiexport.cc asciiexport_factory.cc \
ExportDialogUI.ui ExportDialog.cc

noinst_HEADERS = asciiimport.h asciiimport_factory.h asciiexport.h \
asciiexport_factory.h ExportDialog.h

METASOURCES = AUTO

service_DATA = kword_ascii_import.desktop kword_ascii_export.desktop

servicedir = $(kde_servicesdir)
