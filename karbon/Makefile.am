INCLUDES = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(KOPALETTE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir) \
	-I$(srcdir)/commands \
	-I$(srcdir)/core \
	-I$(srcdir)/dialogs \
	-I$(srcdir)/dockers \
	-I$(srcdir)/render \
	-I$(srcdir)/visitors \
	-I$(srcdir)/widgets \
	$(all_includes)

SUBDIRS = commands visitors core shapes widgets dialogs dockers render . tools plugins data pics templates

# Filters should link to libkarbonbase or libkarboncommon, NOT to libkarbonpart
#
# libkarbonbase was initially intended for it, but filters (e.g. ai) need commands
# (e.g. VTranslateCmd), and commands (e.g. VCommandHistory) need KarbonPart,
# so even moving more stuff to libkarbonbase isn't enough.
# -> I think the whole 'base' stuff can be removed (DF)
lib_LTLIBRARIES = libkarboncommon.la libkarbonbase.la
kde_module_LTLIBRARIES = libkarbonpart.la

libkarboncommon_la_SOURCES = karbon_factory.cc 	karbon_resourceserver.cc 	karbon_part.cc 	karbon_part_iface.cc 	karbon_part_iface.skel 	karbon_view.cc 	karbon_view_iface.cc 	karbon_view_iface.skel karbon_drag.cpp karbon_grid_data.cpp karbon_tool_registry.cc karbon_tool_factory.cc	vtoolcontroller.cc vtool.cc


libkarbonbase_la_SOURCES = \
	karbon_view_base.cc \
	karbon_part_base.cc

libkarbonbase_la_LDFLAGS = $(KDE_RPATH) -version-info 2:0:0 -no-undefined
libkarbonbase_la_LIBADD = $(LIB_KOFFICEUI)

libkarboncommon_la_LDFLAGS = $(KDE_RPATH) $(LIBFREETYPE_RPATH) -no-undefined
libkarboncommon_la_LIBADD = $(LIB_KOFFICECORE) $(LIB_KOFFICEUI) $(LIB_KOPALETTE) $(LIB_KOPAINTER) $(LIB_KOTEXT) \
	dockers/libkarbondockers.la \
	widgets/libkarbonwidgets.la \
	render/libvpainter.la \
	render/xrgbrender/libkarbonxrgbrender.la \
	core/libkarboncore.la \
	shapes/libkarbonshapes.la \
	commands/libkarboncommands.la \
	visitors/libkarbonvisitors.la \
	dialogs/libkarbondialogs.la \
	libkarbonbase.la \
	$(LIBART_LIBS) \
	$(LIBFONTCONFIG_LIBS) \
	$(LIBMAGICK_LIBS) \
	$(LIBFREETYPE_LIBS)

libkarbonpart_la_SOURCES = karbon_factory_init.cc
libkarbonpart_la_LDFLAGS = $(KDE_PLUGIN) $(LIBFREETYPE_RPATH)
libkarbonpart_la_LIBADD = libkarboncommon.la
libkarbonpart_la_METASOURCES = AUTO

bin_PROGRAMS =
kdeinit_LTLIBRARIES = karbon.la
karbon_la_SOURCES = main.cc
karbon_la_LDFLAGS = $(all_libraries) -module -avoid-version
karbon_la_LIBADD = libkarboncommon.la

noinst_HEADERS = \
	karbon_factory.h \
	karbon_part_base.h \
	karbon_part.h \
	karbon_part_iface.h \
	karbon_view_base.h \
	karbon_view.h \
	karbon_drag.h \
	vtoolcontroller.h \
	vtool.h

messages: rc.cpp
	$(EXTRACTRC) data/*.rc >> rc.cpp
	$(XGETTEXT) rc.cpp *.cpp *.cc */*.cc tools/*.h plugins/*/*.cc -o $(podir)/karbon.pot

include $(top_srcdir)/admin/Doxyfile.am

