INCLUDES = -I.. -I../widgets -I../dialogs $(all_includes) $(KOFFICE_INCLUDES)

lib_LTLIBRARIES = libkistools.la
libkistools_la_LDFLAGS = $(all_libraries) -version-info 1:0

libkistools_la_SOURCES = movetool.cc zoomtool.cc brushtool.ccc

#libkistools_la_METASOURCES = 
#include_HEADERS =

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc *.h && mv messages.po ../../po/libkistools.pot
