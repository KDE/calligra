####### General stuff

INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) -I../../libexport $(all_includes)

libhtmlexport_la_LDFLAGS = -module -avoid-version -no-undefined

libhtmlexport_la_LIBADD = $(KOFFICE_LIBS) ../../libexport/libkwordexportfilters.la

####### Files

kde_module_LTLIBRARIES = libhtmlexport.la

libhtmlexport_la_SOURCES = htmlexport.cc htmlexport_factory.cc\
ExportDialog.cc ExportDialogUI.ui ExportFilter.cc

noinst_HEADERS = htmlexport.h htmlexport_factory.h\
ExportDialog.h ExportDialogUI.h ExportFilter.h

METASOURCES = AUTO

service_DATA = kword_html_export.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kwordhtmlexportfilter.pot
