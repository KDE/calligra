####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libkarbonmsodimport_la_LDFLAGS	= -module -avoid-version -no-undefined $(all_libraries)
libkarbonmsodimport_la_LIBADD = $(KOFFICE_LIBS) $(LIBZ)

####### Files

kde_module_LTLIBRARIES = libkarbonmsodimport.la

libkarbonmsodimport_la_SOURCES = msodimport.cc msod.cc

noinst_HEADERS = msodimport.h msod.h

METASOURCES = AUTO

service_DATA = karbon_msod_import.desktop
servicedir = $(kde_servicesdir)
