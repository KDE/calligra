####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)

libasciiimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libasciiimport_la_LIBADD = $(KOFFICE_LIBS)

libasciiexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libasciiexport_la_LIBADD = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la

####### Files

kde_module_LTLIBRARIES = libasciiimport.la libasciiexport.la

libasciiimport_la_SOURCES = asciiimport.cc ImportDialogUI.ui ImportDialog.cc

libasciiexport_la_SOURCES = asciiexport.cc ExportDialogUI.ui ExportDialog.cc

noinst_HEADERS = asciiimport.h ImportDialog.h asciiexport.h ExportDialog.h

METASOURCES = AUTO

service_DATA = kword_ascii_import.desktop kword_ascii_export.desktop

servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kwordasciifilter.pot
