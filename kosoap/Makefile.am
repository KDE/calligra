SUBDIRS = easysoap++ 
INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes) -I./easysoap++/
KDE_CXXFLAGS = -fexceptions -g3
## The part
kde_module_LTLIBRARIES = libkosoappart.la
libkosoappart_la_SOURCES = kosoap_part.cpp soapsession.cpp\
	kosoap_view.cpp kosoap_factory.cpp
libkosoappart_la_LDFLAGS = $(KDE_PLUGIN)
libkosoappart_la_LIBADD = ../lib/kofficeui/libkofficeui.la easysoap++/libeasysoap.la

## The kdeinit loadable module
lib_LTLIBRARIES = kosoap.la
kosoap_la_SOURCES = main.cpp
kosoap_la_LDFLAGS = $(all_libraries) -module -avoid-version -lssl
kosoap_la_LIBADD = ../lib/kofficecore/libkofficecore.la easysoap++/libeasysoap.la

## The executable
bin_PROGRAMS = kosoap
kosoap_SOURCES = dummy.cpp
kosoap_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kosoap_LDADD = kosoap.la

dummy.cpp:
	echo > dummy.cpp

## The plugin
## I need to figure out how to get more ltllibraries here...

METASOURCES = AUTO
noinst_HEADERS =  kosoap_part.h soapsession.h\
	kosoap_view.h kosoap_factory.h kosoap_aboutdata.h

kdelnk_DATA = kosoap.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-kosoap.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = kosoap.rc
rcdir = $(kde_datadir)/kosoap

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cpp kosoap_aboutdata.h -o $(podir)/example.pot
