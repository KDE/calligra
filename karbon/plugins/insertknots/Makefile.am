INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/karbon  -I$(top_srcdir)/karbon/core $(all_includes)

kde_module_LTLIBRARIES = karbon_insertknotsplugin.la

karbon_insertknotsplugin_la_SOURCES = insertknotsplugin.cc
karbon_insertknotsplugin_la_LIBADD = $(LIB_KPARTS) \
					../../libkarboncommon.la

karbon_insertknotsplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

partpluginsdir   = $(kde_datadir)/karbon/kpartplugins
partplugins_DATA = insertknotsplugin.rc

METASOURCES = AUTO

