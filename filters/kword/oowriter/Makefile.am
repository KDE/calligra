####### General stuff

INCLUDES= -I$(srcdir)/../../liboofilter -I$(srcdir)/../libexport \
$(KOFFICE_INCLUDES) $(all_includes)

####### Files
kde_module_LTLIBRARIES = liboowriterimport.la liboowriterexport.la

noinst_LTLIBRARIES = libcommon.la
libcommon_la_SOURCES = conversion.cc

liboowriterimport_la_SOURCES = oowriterimport.cc
liboowriterimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
liboowriterimport_la_LIBADD  = ../../liboofilter/liboofilter.la libcommon.la $(KOFFICE_LIBS)

liboowriterexport_la_SOURCES = oowriterexport.cc ExportFilter.cc
liboowriterexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
liboowriterexport_la_LIBADD  = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la libcommon.la

noinst_HEADERS = ExportFilter.h

METASOURCES = AUTO

service_DATA = kword_oowriter_import.desktop kword_oowriter_export.desktop

servicedir = $(kde_servicesdir)
