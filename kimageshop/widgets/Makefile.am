INCLUDES = -I.. $(all_includes)

lib_LTLIBRARIES = libkiswidgets.la
libkiswidgets_la_LDFLAGS = $(all_libraries) -version-info 1:0
#libkiswidgets_la_LIBADD =

libkiswidgets_la_SOURCES = colorframe.cc colorslider.cc \
	iconchooser.cc integerwidget.cc

libkiswidgets_la_METASOURCES = colorframe.moc colorslider.moc \
	iconchooser.moc integerwidget.moc

#include_HEADERS =

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc *.h && mv messages.po ../../po/libkiswidgets.pot
