INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes) -I$(top_srcdir)/killustrator/share -I$(top_srcdir)/killustrator/filter -I$(top_srcdir)/killustrator

####### Files

bin_PROGRAMS = killustrator
lib_LTLIBRARIES = killustrator.la libkillustrator.la

libkillustrator_la_SOURCES = KIllustrator_doc.cc \
	KIllustrator_shell.cc KIllustrator_view.cc \
	InsertPartTool.cc GPart.cc KIllustrator_factory.cc
libkillustrator_la_LDFLAGS = -avoid-version -module -no-undefined
libkillustrator_la_LIBADD = $(all_libraries) $(top_builddir)/killustrator/share/libkillu.la $(top_builddir)/killustrator/xmlutils/libxml.la $(top_builddir)/killustrator/filter/libfilter.la $(top_builddir)/lib/kofficeui/libkofficeui.la

killustrator_la_SOURCES = main.cc
killustrator_la_LDFLAGS = $(all_libraries) -avoid-version -module
killustrator_la_LIBADD = libkillustrator.la

killustrator_SOURCES = dummy.cc
killustrator_LDFLAGS = $(all_libraries) $(KDE_RPATH)
killustrator_LDADD = killustrator.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS = KIllustrator_shell.h KIllustrator_doc.h \
		 KIllustrator_view.h InsertPartTool.h GPart.h \
		 KIllustrator_factory.h

libkillustrator_la_METASOURCES = AUTO

kdelnk_DATA = killustrator.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-killustrator.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/killustrator
rc_DATA = KIllustrator.rc

# SUBDIRS =  . plugins pics dtd toolbar
SUBDIRS = templates
