
INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../widgets -I$(srcdir)/../dialogs -I../dialogs $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES)

lib_LTLIBRARIES = libkistools.la
libkistools_la_LDFLAGS = $(all_libraries) -version-info 1:0

libkistools_la_SOURCES = movetool.cc zoomtool.cc brushtool.cc

#libkistools_la_METASOURCES = 
#include_HEADERS =

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc *.h && mv messages.po ../../po/libkistools.pot
