####### General stuff

SUBDIRS = import

INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)

libhtmlexport_la_LDFLAGS = -module -avoid-version -no-undefined

libhtmlexport_la_LIBADD = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la

####### Files

kde_module_LTLIBRARIES = libhtmlexport.la

libhtmlexport_la_SOURCES = htmlexport.cc htmlexport_factory.cc\
ExportDialog.cc ExportDialogUI.ui ExportFilterBase.cc\
ExportFilterSpartan.cc ExportFilterDirect.cc ExportFilterStyle.cc\
ExportFilterFullPower.cc

noinst_HEADERS = htmlexport.h htmlexport_factory.h\
ExportDialog.h ExportDialogUI.h\
ExportFilterBase.h ExportFilterSpartan.h ExportFilterDirect.h\
ExportFilterStyle.h ExportFilterFullPower.h

METASOURCES = AUTO

service_DATA = kword_html_export.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kwordhtmlfilter.pot
