
AM_CPPFLAGS = -DQT_NO_ASCII_CAST -DQT_NO_COMPAT

INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes)

graphite_LDFLAGS = $(KDE_RPATH)
graphite_LDADD = ../lib/kofficeui/libkofficeui.la libgraphite.la

libgraphite_la_LDFLAGS = -module -avoid-version -no-undefined

libgraphite_la_LIBADD = ../lib/kofficeui/libkofficeui.la

####### Files

bin_PROGRAMS = graphite
lib_LTLIBRARIES = libgraphite.la

libgraphite_la_SOURCES = graphiteshell.cc graphitepart.cc \
    graphiteview.cc graphitefactory.cc gobject.cc \
    gcanvas.cc gobjectfactory.cc gcommand.cc gline.cc

graphite_SOURCES = main.cc

noinst_HEADERS =  graphiteglobal.h graphiteshell.h \
    graphitepart.h graphiteview.h graphitefactory.h \
    gobject.h gcanvas.h gobjectfactory.h gcommand.h \
    gtool.h gline.h

libgraphite_la_METASOURCES = AUTO

kdelnk_DATA = graphite.desktop
kdelnkdir = $(kde_appsdir)/Applications/KOffice

kdemime_DATA = x-graphite.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = graphite.rc
rcdir = $(kde_datadir)/graphite

# SUBDIRS =  . plugins pics dtd toolbar

messages:
#   $(XGETTEXT) *.cc -o $(podir)/graphite.pot
