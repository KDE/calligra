INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes)

kimageshop_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kimageshop_LDADD = ../lib/kofficeui/libkofficeui.la

libkimageshop_la_LDFLAGS = $(all_libraries) -avoid-version -module
libkimageshop_la_LIBADD =  core/libkiscore.la ui/libkisui.la tools/libkistools.la \
	../lib/kofficeui/libkofficeui.la

SUBDIRS = core tools ui . plugins data pics

bin_PROGRAMS = kimageshop
lib_LTLIBRARIES = libkimageshop.la

libkimageshop_la_SOURCES = dummy.cc

kimageshop_SOURCES = main.cc

kdelnk_DATA = kimageshop.desktop
kdelnkdir = $(kde_appsdir)/Applications/KOffice

kdemime_DATA = x-kimageshop.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kimageshop
rc_DATA = kimageshop.rc

check_PROGRAMS = linktest
linktest_SOURCES = $(libkimageshop_la_SOURCES) $(kimageshop_SOURCES)
linktest_LDADD = $(lib_LTLIBRARIES)
linktest_LDFLAGS = $(libkimageshop_LDFLAGS)

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc */*.cc && mv messages.po $(podir)/kimageshop.pot

