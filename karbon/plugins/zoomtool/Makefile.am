INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/karbon  -I$(top_srcdir)/karbon/core $(all_includes)

kde_module_LTLIBRARIES = karbon_zoomtoolplugin.la

karbon_zoomtoolplugin_la_SOURCES = zoomtoolplugin.cc
karbon_zoomtoolplugin_la_LIBADD = $(LIB_KPARTS) $(LIB_KOFFICEUI) \
				../../libkarbonpart.la

karbon_zoomtoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

datadir   = $(kde_datadir)/karbon/kpartplugins
data_DATA = zoomtoolplugin.rc

METASOURCES = AUTO

