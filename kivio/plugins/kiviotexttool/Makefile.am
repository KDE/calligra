#1########################################################!@#$
####   Makefile.am generated automatically by KDEStudio   ####
#### WARNING! All changes made in this part will be lost! ####
##############################################################

lib_LTLIBRARIES =libkiviotexttool.la


libkiviotexttool_la_LDFLAGS = -version-info 

INCLUDES = -I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tklib\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	$(KOFFICE_INCLUDES) $(all_includes)
libkiviotexttool_la_METASOURCES = AUTO
libkiviotexttool_la_SOURCES = \
		plugin.cpp\
		tool_text.cpp
EXTRA_DIST = \
		plugin.h\
		plugin.cpp\
		kiviotexttool.desktop\
		kiviotexttool.rc\
		tool_text.cpp\
		tool_text.h\
		kivio_text.xpm\
		kivio_text_cursor.xpm\
		Text.sml\
		Text.xpm
##############################################################
####                  END KDEStudio part                  ####
#2########################################################!@#$
libkiviotexttool_la_LDFLAGS = -avoid-version -module -no-undefined
libkiviotexttool_la_LIBADD = $(all_libraries) $(top_builddir)/kivio/kiviopart/libkiviopart.la

rc_DATA = kiviotexttool.rc
rcdir = $(kde_datadir)/kivio/kpartplugins

desktop_DATA = kiviotexttool.desktop
desktopdir = $(kde_servicesdir)

data_DATA = kivio_text.xpm kivio_text_cursor.xpm
datadir = $(kde_datadir)/kivio/pics

stencil_DATA = Text.sml Text.xpm
stencildir = $(kde_datadir)/kivio/autoloadStencils
