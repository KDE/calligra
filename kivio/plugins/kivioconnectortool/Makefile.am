#1########################################################!@#$
####   Makefile.am generated automatically by KDEStudio   ####
#### WARNING! All changes made in this part will be lost! ####
##############################################################
SUBDIRS = straight_connector 
lib_LTLIBRARIES =libkivioconnectortool.la


libkivioconnectortool_la_LDFLAGS = -version-info

INCLUDES = $(all_includes)\
		-I$(top_srcdir)/kivio/kiviopart\
		-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
		-I$(top_srcdir)/kivio/kiviopart/tklib\
		-I$(top_srcdir)/kivio/kiviopart/tools\
		-I$(top_srcdir)/kivio/plugins/kivioconnectortool/straight_connector
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
##############################################################
####                  END KDEStudio part                  ####
#2########################################################!@#$
libkivioconnectortool_la_LDFLAGS = -avoid-version -module -no-undefined
libkivioconnectortool_la_LIBADD = $(all_libraries) $(top_builddir)/kivio/kiviopart/libkiviopart.la

rc_DATA = kivioconnectortool.rc
rcdir = $(kde_datadir)/kivio/kpartplugins

desktop_DATA = kivioconnectortool.desktop
desktopdir = $(kde_servicesdir)

data_DATA = kivio_connector.xpm kivio_connector_cursor1.xpm kivio_connector_cursor2.xpm
datadir = $(kde_datadir)/kivio/pics

