INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/karbon  -I$(top_srcdir)/karbon/core $(all_includes)

kde_module_LTLIBRARIES = karbon_roundcornersplugin.la

karbon_roundcornersplugin_la_SOURCES = roundcornersplugin.cc
karbon_roundcornersplugin_la_LIBADD = $(LIB_KPARTS) \
					../../libkarbonpart.la

karbon_roundcornersplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

datadir   = $(kde_datadir)/karbon/kpartplugins
data_DATA = roundcornersplugin.rc

METASOURCES = AUTO

