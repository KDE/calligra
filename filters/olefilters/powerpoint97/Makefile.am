####### General stuff

INCLUDES= -I$(srcdir) -I$(srcdir)/../lib $(all_includes)
LDFLAGS	= $(all_libraries)

####### Files

noinst_LTLIBRARIES = libpowerpoint97.la

libpowerpoint97_la_SOURCES = powerpointfilter.cc powerpoint.cc

noinst_HEADERS = powerpointfilter.h powerpoint.h

METASOURCES = AUTO

service_DATA = ole_powerpoint97_import.desktop
servicedir = $(kde_servicesdir)
