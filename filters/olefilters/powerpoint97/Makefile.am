####### General stuff

INCLUDES= -I$(srcdir) -I$(srcdir)/../lib $(KOFFICE_INCLUDES) $(all_includes)
LDFLAGS	= $(all_libraries)

####### Files

noinst_LTLIBRARIES = libpowerpoint97.la

libpowerpoint97_la_SOURCES = powerpointfilter.cc powerpoint.cc pptdoc.cc pptxml.cc

noinst_HEADERS = powerpointfilter.h powerpoint.h pptdoc.h pptxml.h

METASOURCES = AUTO

service_DATA = ole_powerpoint97_import.desktop
servicedir = $(kde_servicesdir)
