INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) -I$(top_srcdir)/killustrator/share -I$(top_srcdir)/killustrator/filter -I$(top_srcdir)/killustrator $(all_includes)

####### Files

bin_PROGRAMS = killustrator
lib_LTLIBRARIES = libkillustratorpart.la killustrator.la

libkillustratorpart_la_SOURCES = KIllustrator_doc.cc \
	KIllustrator_view.cc \
	InsertPartTool.cc GPart.cc KIllustrator_factory.cc
libkillustratorpart_la_LDFLAGS = -avoid-version -module -no-undefined
libkillustratorpart_la_LIBADD = $(all_libraries) $(top_builddir)/killustrator/share/libkillu.la $(top_builddir)/killustrator/filter/libfilter.la $(top_builddir)/lib/kofficeui/libkofficeui.la

killustrator_la_SOURCES = main.cc
killustrator_la_LDFLAGS = $(all_libraries) -avoid-version -module
killustrator_la_LIBADD = libkillustratorpart.la

killustrator_SOURCES = dummy.cc
killustrator_LDFLAGS = $(all_libraries) $(KDE_RPATH)
killustrator_LDADD = killustrator.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS = KIllustrator_doc.h \
		 KIllustrator_view.h InsertPartTool.h GPart.h \
		 KIllustrator_factory.h

libkillustratorpart_la_METASOURCES = AUTO

kdelnk_DATA = killustrator.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-killustrator.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/killustrator
rc_DATA = KIllustrator.rc

SUBDIRS = templates

