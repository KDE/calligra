INCLUDES = -I$(srcdir)/../lib $(KOFFICE_INCLUDES) $(all_includes)

noinst_PROGRAMS = dbsample
dbsample_SOURCES = main.cpp mainform.ui
dbsample_LDFLAGS = $(all_libraries) $(KDE_RPATH)
dbsample_LDADD = ../lib/libkugarlib.la $(LIB_KIO) $(LIB_KDEPRINT)

dbsample_METASOURCES = AUTO

EXTRA_DIST = \
	kugardb.sql \
	kugartemplate.dtd \
	sampledb1.kut
