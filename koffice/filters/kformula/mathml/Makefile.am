####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(all_includes)
libkfomathmlexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libkfomathmlexport_la_LIBADD    = $(KOFFICE_LIBS) $(LIB_KFORMULA)
libkfomathmlimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libkfomathmlimport_la_LIBADD    = $(KOFFICE_LIBS) $(LIB_KFORMULA)

####### Files

kde_module_LTLIBRARIES = libkfomathmlexport.la libkfomathmlimport.la

libkfomathmlexport_la_SOURCES = mathmlexport.cc

libkfomathmlimport_la_SOURCES = mathmlimport.cc

noinst_HEADERS = mathmlexport.h mathmlimport.h


METASOURCES = AUTO

service_DATA = kformula_mathml_export.desktop kformula_mathml_import.desktop
servicedir = $(kde_servicesdir)
