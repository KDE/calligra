####### General stuff

INCLUDES = $(KOFFICE_INCLUDES) -I$(srcdir) -I$(srcdir)/../lib $(all_includes)
LDFLAGS = $(all_libraries)

####### Files

noinst_LTLIBRARIES = libwinword97.la

libwinword97_la_SOURCES = wordfilter.cc winworddoc.cc \
msdrawing.cc \
msword.cc mswordgenerated.cc properties.cc document.cc

noinst_HEADERS = wordfilter.h winworddoc.h \
msdrawing.h \
msword.h mswordgenerated.h properties.h document.h

METASOURCES = AUTO

service_DATA = ole_winword97_import.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/olefilterswinword97filter.pot
