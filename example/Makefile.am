
INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes)

####### Files

bin_PROGRAMS = example
lib_LTLIBRARIES = example.la libexamplepart.la

libexamplepart_la_SOURCES = example_shell.cc example_part.cc \
                            example_view.cc example_factory.cc
libexamplepart_la_LDFLAGS = -module -avoid-version -no-undefined
libexamplepart_la_LIBADD = ../lib/kofficeui/libkofficeui.la
libexamplepart_la_METASOURCES = AUTO

example_la_SOURCES = main.cc
example_la_LDFLAGS = $(all_libraries) -module -avoid-version
example_la_LIBADD = libexamplepart.la

example_SOURCES = dummy.cc
example_LDFLAGS = $(all_libraries) $(KDE_RPATH)
example_LDADD = example.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS =  example_shell.h example_part.h \
		  example_view.h example_factory.h

kdelnk_DATA = example.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-example.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = example.rc
rcdir = $(kde_datadir)/example
