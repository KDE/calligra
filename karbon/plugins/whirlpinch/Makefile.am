INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/karbon -I$(top_srcdir)/karbon/core $(KOPAINTER_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES = karbon_whirlpinchplugin.la

karbon_whirlpinchplugin_la_SOURCES = whirlpinchplugin.cc
karbon_whirlpinchplugin_la_LIBADD = $(LIB_KPARTS) $(LIB_KOPAINTER) \
				../../libkarboncommon.la


karbon_whirlpinchplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

partpluginsdir   = $(kde_datadir)/karbon/kpartplugins
partplugins_DATA = whirlpinchplugin.rc

METASOURCES = AUTO

