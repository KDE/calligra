####### General stuff

INCLUDES = -I$(srcdir) -I$(srcdir)/../lib $(all_includes)
LDFLAGS = $(all_libraries)

####### Files

noinst_LTLIBRARIES = libwinword97.la

libwinword97_la_SOURCES = wordfilter.cc winworddoc.cc \
msword.cc mswordgenerated.cc paragraph.cc

noinst_HEADERS = wordfilter.h winworddoc.h \
msword.h mswordgenerated.h paragraph.h

METASOURCES = AUTO

service_DATA = ole_winword97_import.desktop
servicedir = $(kde_servicesdir)
