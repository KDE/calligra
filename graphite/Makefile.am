
AM_CPPFLAGS = -DQT_NO_CAST_ASCII

INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes)

####### Files

bin_PROGRAMS = graphite
lib_LTLIBRARIES = graphite.la libgraphitepart.la

libgraphitepart_la_SOURCES = graphiteshell.cc graphitepart.cc \
    graphiteview.cc graphitefactory.cc gobject.cc \
    gcanvas.cc gobjectfactory.cc gcommand.cc gline.cc \
    graphiteglobal.cc ggroup.cc gprimitivecmds.cc
libgraphitepart_la_LDFLAGS = -module -avoid-version -no-undefined
libgraphitepart_la_LIBADD = ../lib/kofficeui/libkofficeui.la
libgraphitepart_la_METASOURCES = AUTO

graphite_la_SOURCES = main.cc
graphite_la_LDFLAGS = $(all_libraries) -module -avoid-version
graphite_la_LIBADD = libgraphitepart.la

graphite_SOURCES = dummy.cc
graphite_LDFLAGS = $(all_libraries) $(KDE_RPATH)
graphite_LDADD = graphite.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS =  graphiteglobal.h graphiteshell.h \
    graphitepart.h graphiteview.h graphitefactory.h \
    gobject.h gcanvas.h gobjectfactory.h gcommand.h \
    gline.h ggroup.h gprimitivecmds.h

kdelnk_DATA = graphite.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-graphite.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = graphite.rc
rcdir = $(kde_datadir)/graphite

# SUBDIRS =  . plugins pics dtd toolbar

messages:
	$(XGETTEXT) rc.cpp *.cc -o $(podir)/graphite.pot
