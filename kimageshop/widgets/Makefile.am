
INCLUDES = -I$(srcdir)/.. $(all_includes)

noinst_LTLIBRARIES = libkiswidgets.la
# libkiswidgets_a_LDFLAGS = $(all_libraries) -version-info 1:0
#libkiswidgets_a_LIBADD =

libkiswidgets_la_SOURCES = colorframe.cc colorslider.cc \
	iconchooser.cc integerwidget.cc blendchooser.cc

libkiswidgets_la_METASOURCES = colorframe.moc colorslider.moc \
	iconchooser.moc integerwidget.moc blendchooser.moc

#include_HEADERS =

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc *.h && mv messages.po ../../po/libkiswidgets.pot
