INCLUDES = -I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	$(KOTEXT_INCLUDES)\
	$(KOFFICE_INCLUDES) $(all_includes)

METASOURCES = AUTO

noinst_LTLIBRARIES =  libpolyline_connector.la
libpolyline_connector_la_LIBADD = $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la

libpolyline_connector_la_LDFLAGS = $(all_libraries)
libpolyline_connector_la_SOURCES = kiviopolylineconnector.cpp polylineconnectorspawner.cpp

noinst_HEADERS = kiviopolylineconnector.h
