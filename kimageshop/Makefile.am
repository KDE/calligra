INCLUDES  = -I$(srcdir)/widgets -I$(srcdir)/dialogs -I$(srcdir)/tools\
 $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes) 

kimageshop_LDFLAGS = $(all_libraries) $(KDE_RPATH) 
kimageshop_LDADD = ../lib/kofficeui/libkofficeui.la

libkimageshop_la_LDFLAGS = $(all_libraries) -avoid-version -module
libkimageshop_la_LIBADD =  widgets/libkiswidgets.la dialogs/libkisdialogs.la tools/libkistools.la ../lib/kofficeui/libkofficeui.la $(LIB_KIMGIO) 

SUBDIRS = widgets dialogs tools . data pics

####### Files

bin_PROGRAMS = kimageshop
lib_LTLIBRARIES = libkimageshop.la

libkimageshop_la_SOURCES = kimageshop_shell.cc kimageshop_doc.cc kimageshop_view.cc \
	kimageshop_factory.cc kimageshop_config.cc kimageshop_undo.cc brush.cc \
	channeldata.cc channel.cc misc.cc layer.cc tool.cc  vec.cc color.cc

kimageshop_SOURCES = main.cc


noinst_HEADERS =  kimageshop_global.h  kimageshop_shell.h kimageshop_doc.h \
	kimageshop_view.h kimageshop_factory.h kimageshop_config.h \
	kimageshop_undo.h vec.h color.h channeldata.h channel.h misc.h layer.h tool.h

libkimageshop_la_METASOURCES = kimageshop_shell.moc kimageshop_doc.moc \
	kimageshop_view.moc kimageshop_factory.moc kimageshop_config.moc layer.moc tool.moc

kdelnk_DATA = kimageshop.desktop
kdelnkdir = $(kde_appsdir)/KOffice

kdemime_DATA = x-kimageshop.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kimageshop
rc_DATA = kimageshop.rc kimageshop_shell.rc

# SUBDIRS =  . plugins pics dtd toolbar

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kimageshop.pot
