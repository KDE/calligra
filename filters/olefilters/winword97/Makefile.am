####### General stuff

INCLUDES= -I$(srcdir) -I$(srcdir)/../lib $(all_includes)
LDFLAGS	= $(all_libraries) -version-info 1:0:0

####### Files

lib_LTLIBRARIES = libwinword97.la

libwinword97_la_SOURCES = wordfilter.cc winworddoc.cc \
kworddoc.cc paragraph.cc picture.cc style.cc \
stylesheet.cc misc.cc

noinst_HEADERS = winworddoc.h kworddoc.h fib.h wordfilter.h \
paragraph.h picture.h style.h stylesheet.h misc.h

METASOURCES = AUTO

service_DATA = ole_winword97_import.desktop
servicedir = $(kde_servicesdir)

data_DATA = x-winword97.desktop
datadir = $(kde_mimedir)/application
