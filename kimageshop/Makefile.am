INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes)

## The part
kde_module_LTLIBRARIES = libkrayonpart.la
libkrayonpart_la_SOURCES = dummy.cc

libkrayonpart_la_LDFLAGS = $(KDE_PLUGIN)
libkrayonpart_la_LIBADD = core/libkiscore.la ui/libkisui.la tools/libkistools.la \
	../lib/kofficeui/libkofficeui.la
METASOURCES = AUTO

## The kdeinit loadable module
lib_LTLIBRARIES = krayon.la
krayon_la_SOURCES = main.cc
krayon_la_LDFLAGS = $(all_libraries) -avoid-version -module
krayon_la_LIBADD = libkrayonpart.la

## The executable
bin_PROGRAMS = krayon
krayon_SOURCES = dummy.cc
krayon_LDFLAGS = $(all_libraries) $(KDE_RPATH)
krayon_LDADD = krayon.la

dummy.cc:
	echo > dummy.cc

kdelnk_DATA = krayon.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-krayon.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/krayon
rc_DATA = krayon.rc

SUBDIRS = core tools ui . dtd plugins data pics

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc */*.cc -o $(podir)/krayon.pot

