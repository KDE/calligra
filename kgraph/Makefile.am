
INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes)

kgraph_LDFLAGS = $(KDE_RPATH)
kgraph_LDADD = ../lib/kofficeui/libkofficeui.la libkgraph.la

libkgraph_la_LDFLAGS = -module -avoid-version -no-undefined

libkgraph_la_LIBADD = ../lib/kofficeui/libkofficeui.la

####### Files

bin_PROGRAMS = kgraph
lib_LTLIBRARIES = libkgraph.la

libkgraph_la_SOURCES = kgraph_shell.cc kgraph_part.cc \
    kgraph_view.cc kgraph_factory.cc kgobject.cc \
    kgcanvas.cc kgobjectpool.cc

kgraph_SOURCES = main.cc

noinst_HEADERS =  kgraph_global.h kgraph_shell.h \
    kgraph_part.h kgraph_view.h kgraph_factory.h \
    kgobject.h kgcanvas.h kgobjectpool.h

libkgraph_la_METASOURCES = AUTO

kdelnk_DATA = kgraph.desktop
kdelnkdir = $(kde_appsdir)/Applications/KOffice

kdemime_DATA = x-kgraph.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = kgraph.rc
rcdir = $(kde_datadir)/kgraph

# SUBDIRS =  . plugins pics dtd toolbar

messages:
#   $(XGETTEXT) *.cc -o $(podir)/kgraph.pot
