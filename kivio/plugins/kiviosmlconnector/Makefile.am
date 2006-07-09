SUBDIRS=. sml_connector
kde_module_LTLIBRARIES =libkiviosmlconnector.la

INCLUDES = -I$(top_srcdir)/kivio/kiviopart/config \
	-I$(top_builddir)/kivio/kiviopart/config \
	-I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	-I$(top_srcdir)/kivio/plugins/kiviosmlconnector/sml_connector \
	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)

METASOURCES = AUTO
libkiviosmlconnector_la_SOURCES = plugin.cpp tool_connector.cpp
libkiviosmlconnector_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libkiviosmlconnector_la_LIBADD = $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la



rc_DATA = kiviosmlconnector.rc
rcdir = $(kde_datadir)/kivio/kpartplugins

desktop_DATA = kiviosmlconnector.desktop
desktopdir = $(kde_servicesdir)

