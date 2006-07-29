####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

####### Files
kde_module_LTLIBRARIES = libgenerickofilter.la 


libgenerickofilter_la_SOURCES = generic_filter.cc
libgenerickofilter_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libgenerickofilter_la_LIBADD  = $(KOFFICE_LIBS) $(LIB_KIO)

METASOURCES = AUTO

noinst_HEADERS = generic_filter.h

service_DATA = generic_filter.desktop
servicedir = $(kde_servicesdir)
