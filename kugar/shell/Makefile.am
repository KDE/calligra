INCLUDES = -I$(srcdir)/../lib $(all_includes)

bin_PROGRAMS = kugar
kugar_SOURCES = main.cpp report.cpp
kugar_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kugar_LDADD = ../lib/libkugar.la $(LIB_KIO) -lkdeprint

kugar_METASOURCES = AUTO

rcdir = $(kde_datadir)/kugar
rc_DATA = kugarui.rc

EXTRA_DIST = \
	report.h \
	$(rc_DATA)
