####### General stuff

INCLUDES= -I$(srcdir) $(KWMF_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes)
libmsodimport_la_LDFLAGS	= -module -avoid-version -no-undefined $(all_libraries)
libmsodimport_la_LIBADD = $(KOFFICE_LIBS) $(LIB_KWMF)

####### Files

kde_module_LTLIBRARIES = libmsodimport.la

libmsodimport_la_SOURCES = msodimport.cc msod.cc

noinst_HEADERS = msodimport.h msod.h

METASOURCES = AUTO

service_DATA = kontour_msod_import.desktop
servicedir = $(kde_servicesdir)
