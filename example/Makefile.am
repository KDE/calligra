
INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes)

example_LDFLAGS = $(KDE_RPATH) 
example_LDADD = ../lib/kofficeui/libkofficeui.la 

libexample_la_LDFLAGS = -module -avoid-version

libexample_la_LIBADD = ../lib/kofficeui/libkofficeui.la

####### Files

bin_PROGRAMS = example
lib_LTLIBRARIES = libexample.la

libexample_la_SOURCES = example_shell.cc example_part.cc \
	example_view.cc example_factory.cc

example_SOURCES = main.cc

noinst_HEADERS =  example_global.h \
		  example_shell.h example_part.h \
		  example_view.h example_factory.h

libexample_la_METASOURCES = example_shell.moc example_part.moc \
			       example_view.moc example_factory.moc

kdelnk_DATA = example.desktop
kdelnkdir = $(kde_appsdir)/Applications/KOffice

kdemime_DATA = x-example.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = example.rc
rcdir = $(kde_datadir)/example/example.rc

# SUBDIRS =  . plugins pics dtd toolbar

