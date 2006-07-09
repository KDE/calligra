SUBDIRS = select_pics
kde_module_LTLIBRARIES =libkivioselecttool.la


INCLUDES = -I$(top_srcdir)/kivio/kiviopart/config \
	-I$(top_builddir)/kivio/kiviopart/config \
	-I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
libkivioselecttool_la_METASOURCES = AUTO
libkivioselecttool_la_SOURCES = \
		plugin.cpp\
		tool_select.cpp
EXTRA_DIST = \
		plugin.cpp\
		plugin.h\
		kivioselecttool.desktop\
		tool_select.cpp\
		tool_select.h\
		kivioselecttool.rc
libkivioselecttool_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
libkivioselecttool_la_LIBADD = $(LIB_KOTEXT) $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la

rc_DATA = kivioselecttool.rc
rcdir = $(kde_datadir)/kivio/kpartplugins

desktop_DATA = kivioselecttool.desktop
desktopdir = $(kde_servicesdir)

