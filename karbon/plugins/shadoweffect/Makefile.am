INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) -I$(top_srcdir)/karbon  -I$(top_srcdir)/karbon/core $(all_includes)

kde_module_LTLIBRARIES = karbon_shadoweffectplugin.la

karbon_shadoweffectplugin_la_SOURCES = shadoweffectplugin.cc vshadowdecorator.cc 
karbon_shadoweffectplugin_la_LIBADD = $(LIB_KPARTS) $(LIB_KOFFICEUI) \
					../../libkarboncommon.la

karbon_shadoweffectplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

partpluginsdir   = $(kde_datadir)/karbon/kpartplugins
partplugins_DATA = shadoweffectplugin.rc

METASOURCES = AUTO

