####### General stuff

INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../../libexport $(all_includes)

librtfexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined

librtfexport_la_LIBADD = $(KOFFICE_LIBS) ../../libexport/libkwordexportfilters.la

####### Files

kde_module_LTLIBRARIES = librtfexport.la

librtfexport_la_SOURCES = rtfexport.cc ExportFilter.cc

noinst_HEADERS = rtfexport.h ExportFilter.h

METASOURCES = AUTO

service_DATA = kword_rtf_export.desktop
servicedir = $(kde_servicesdir)
