INCLUDES  = -I$(srcdir)/core/ $(all_includes)

SUBDIRS = core . data pics

bin_PROGRAMS = karbon
lib_LTLIBRARIES = \
	karbon.la \
	libkarbonpart.la

libkarbonpart_la_SOURCES = \
	karbon_part.cc \
	karbon_view.cc \
	karbon_factory.cc
libkarbonpart_la_LDFLAGS = -module -avoid-version -no-undefined
libkarbonpart_la_LIBADD = \
	../../koffice/lib/kofficeui/libkofficeui.la \
	./core/libkarboncore.la
libkarbonpart_la_METASOURCES = AUTO

karbon_la_SOURCES = main.cc
karbon_la_LDFLAGS = $(all_libraries) -module -avoid-version
karbon_la_LIBADD = libkarbonpart.la

karbon_SOURCES = dummy.cc
karbon_LDFLAGS = $(all_libraries) $(KDE_RPATH)
karbon_LDADD = karbon.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS = \
	karbon_part.h \
	karbon_view.h \
	karbon_factory.h
