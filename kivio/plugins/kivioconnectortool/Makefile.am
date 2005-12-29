SUBDIRS = straight_connector .
kde_module_LTLIBRARIES =libkivioconnectortool.la


INCLUDES = -I$(top_srcdir)/kivio/kiviopart/config \
	-I$(top_builddir)/kivio/kiviopart/config \
	-I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	-I$(top_srcdir)/kivio/plugins/kivioconnectortool/straight_connector \
	-I$(top_srcdir)/kivio/plugins/kivioconnectortool/polyline_connector \
	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
libkivioconnectortool_la_METASOURCES = AUTO
libkivioconnectortool_la_SOURCES = \
		plugin.cpp\
		tool_connector.cpp
EXTRA_DIST = \
		plugin.h\
		plugin.cpp\
		kivio_connector.xpm\
		tool_connector.h\
		tool_connector.cpp\
		kivioconnectortool.rc\
		kivioconnectortool.desktop\
		kivio_connector_cursor2.xpm\
		kivio_connector_cursor1.xpm

libkivioconnectortool_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
libkivioconnectortool_la_LIBADD = $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la

rc_DATA = kivioconnectortool.rc
rcdir = $(kde_datadir)/kivio/kpartplugins

desktop_DATA = kivioconnectortool.desktop
desktopdir = $(kde_servicesdir)

kiviopics_DATA = kivio_connector_cursor1.png kivio_connector_cursor2.png
kiviopicsdir = $(kde_datadir)/kivio/pics

kivioicondir = $(kde_datadir)/kivio/icons
kivioicon_ICON = AUTO
