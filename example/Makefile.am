
INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes)

## The part
kde_module_LTLIBRARIES = libexamplepart.la
libexamplepart_la_SOURCES = example_part.cc \
	example_view.cc example_factory.cc
libexamplepart_la_LDFLAGS = $(KDE_PLUGIN)
libexamplepart_la_LIBADD = ../lib/kofficeui/libkofficeui.la

## The kdeinit loadable module
lib_LTLIBRARIES = example.la
example_la_SOURCES = main.cc
example_la_LDFLAGS = $(all_libraries) -module -avoid-version
example_la_LIBADD = ../lib/kofficecore/libkofficecore.la

## The executable
bin_PROGRAMS = example
example_SOURCES = dummy.cc
example_LDFLAGS = $(all_libraries) $(KDE_RPATH)
example_LDADD = example.la

dummy.cc:
	echo > dummy.cc

METASOURCES = AUTO
noinst_HEADERS =  example_part.h \
	example_view.h example_factory.h example_aboutdata.h

kdelnk_DATA = example.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-example.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = example.rc
rcdir = $(kde_datadir)/example

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc example_aboutdata.h -o $(podir)/example.pot
