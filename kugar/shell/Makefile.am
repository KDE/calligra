
################# THIS DIRECTORY IS NOT COMPILED ANYMORE ####################
################# THE koshell EXECUTABLE IS CREATED BY ../part ##############

INCLUDES = -I$(srcdir)/../lib $(KOFFICE_INCLUDES) $(all_includes)

bin_PROGRAMS =
lib_LTLIBRARIES =

kdeinit_LTLIBRARIES = kugar.la

kugar_la_SOURCES = main.cpp report.cpp
kugar_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module -avoid-version
kugar_la_LIBADD = ../lib/libkugar.la $(LIB_KIO) $(LIB_KDEPRINT)

kugar_METASOURCES = AUTO

rcdir = $(kde_datadir)/kugar
rc_DATA = kugarui.rc

EXTRA_DIST = \
	report.h \
	$(rc_DATA)
