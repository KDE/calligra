INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/karbon  -I$(top_srcdir)/karbon/core $(all_includes)

kde_module_LTLIBRARIES = karbon_imagetoolplugin.la

karbon_imagetoolplugin_la_SOURCES = imagetoolplugin.cc
karbon_imagetoolplugin_la_LIBADD = $(LIB_KPARTS) $(LIB_KOFFICEUI) \
				../../libkarbonpart.la

karbon_imagetoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

datadir   = $(kde_datadir)/karbon/kpartplugins
data_DATA = imagetoolplugin.rc

METASOURCES = AUTO

