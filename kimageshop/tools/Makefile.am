INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../widgets -I$(srcdir)/../dialogs $(KOFFICE_INCLUDES) $(all_includes)

noinst_LTLIBRARIES = libkistools.la

libkistools_la_SOURCES = movetool.cc zoomtool.cc brushtool.cc gradienttool.cc pentool.cc colorpicker.cc

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc *.h && mv messages.po ../../po/libkistools.pot
