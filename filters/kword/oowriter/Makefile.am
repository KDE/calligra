####### General stuff

INCLUDES= -I$(srcdir)/../../liboofilter \
$(KOFFICE_INCLUDES) $(all_includes)

####### Files
kde_module_LTLIBRARIES = liboowriterimport.la

noinst_LTLIBRARIES = libcommon.la
libcommon_la_SOURCES = conversion.cc

liboowriterimport_la_SOURCES = oowriterimport.cc
liboowriterimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
liboowriterimport_la_LIBADD  = ../../liboofilter/liboofilter.la libcommon.la $(KOFFICE_LIBS)


METASOURCES = AUTO

service_DATA = kword_oowriter_import.desktop

servicedir = $(kde_servicesdir)
