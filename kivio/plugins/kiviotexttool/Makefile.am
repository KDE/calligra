kde_module_LTLIBRARIES =libkiviotexttool.la


INCLUDES = -I$(top_srcdir)/kivio/kiviopart/config \
	-I$(top_builddir)/kivio/kiviopart/config \
	-I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
libkiviotexttool_la_METASOURCES = AUTO
libkiviotexttool_la_SOURCES = \
		plugin.cpp\
		tool_text.cpp kivio_stenciltexteditorui.ui stenciltexteditor.cpp
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
libkiviotexttool_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
libkiviotexttool_la_LIBADD =$(LIB_KOTEXT) $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la

rc_DATA = kiviotexttool.rc
rcdir = $(kde_datadir)/kivio/kpartplugins

desktop_DATA = kiviotexttool.desktop
desktopdir = $(kde_servicesdir)

kiviopics_DATA = kivio_text.xpm kivio_text_cursor.xpm
kiviopicsdir = $(kde_datadir)/kivio/pics

stencil_DATA = Text.sml Text.xpm
stencildir = $(kde_datadir)/kivio/autoloadStencils
