
AM_CPPFLAGS = -DQT_NO_CAST_ASCII

INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes)

## The part
kde_module_LTLIBRARIES = libgraphitepart.la
libgraphitepart_la_SOURCES = graphitepart.cc \
    graphiteview.cc graphitefactory.cc gobject.cc \
    gcanvas.cc gobjectfactory.cc gline.cc \
    graphiteglobal.cc ggroup.cc gprimitivecmds.cc \
    gbackground.cc ruler.cc pagelayoutdia.ui \
    pagelayoutdia_impl.cc myspinbox.cc
libgraphitepart_la_LDFLAGS = $(KDE_PLUGIN)
libgraphitepart_la_LIBADD = $(LIB_KOFFICEUI)
libgraphitepart_la_METASOURCES = AUTO

## The kdeinit loadable module
lib_LTLIBRARIES = graphite.la
graphite_la_SOURCES = main.cc
graphite_la_LDFLAGS = $(all_libraries) -module -avoid-version
graphite_la_LIBADD = $(LIB_KOFFICECORE)

## The executable
bin_PROGRAMS = graphite
graphite_SOURCES = dummy.cc
graphite_LDFLAGS = $(all_libraries) $(KDE_RPATH)
graphite_LDADD = graphite.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS =  graphiteglobal.h \
    graphitepart.h graphiteview.h graphitefactory.h \
    gobject.h gcanvas.h gobjectfactory.h \
    gline.h ggroup.h gprimitivecmds.h \
    gbackground.h ruler.h pagelayoutdia_impl.h \
    myspinbox.h gaboutdata.h

kdelnk_DATA = graphite.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-graphite.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = graphite.rc
rcdir = $(kde_datadir)/graphite

SUBDIRS =  . tests

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc gaboutdata.h -o $(podir)/graphite.pot
