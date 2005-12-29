SUBDIRS = zoom_pics
kde_module_LTLIBRARIES =libkiviozoomtool.la

INCLUDES = -I$(top_srcdir)/kivio/kiviopart/config \
	-I$(top_builddir)/kivio/kiviopart/config \
	-I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
libkiviozoomtool_la_METASOURCES = AUTO
libkiviozoomtool_la_SOURCES = \
		plugin.cpp\
		tool_zoom.cpp
EXTRA_DIST = \
		plugin.cpp\
		plugin.h\
		kiviozoomtool.desktop\
		kiviozoomtool.rc\
		tool_zoom.cpp\
		tool_zoom.h
libkiviozoomtool_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
libkiviozoomtool_la_LIBADD = $(LIB_KOTEXT) $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la

rc_DATA = kiviozoomtool.rc
rcdir = $(kde_datadir)/kivio/kpartplugins

desktop_DATA = kiviozoomtool.desktop
desktopdir = $(kde_servicesdir)

