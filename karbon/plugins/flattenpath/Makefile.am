INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/karbon  -I$(top_srcdir)/karbon/core $(all_includes)

kde_module_LTLIBRARIES = karbon_flattenpathplugin.la

karbon_flattenpathplugin_la_SOURCES = flattenpathplugin.cc
karbon_flattenpathplugin_la_LIBADD = ../../libkarbonpart.la $(LIB_KPARTS)
karbon_flattenpathplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

datadir   = $(kde_datadir)/karbon/kpartplugins
data_DATA = flattenpathplugin.rc

METASOURCES = AUTO

