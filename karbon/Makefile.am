INCLUDES = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir) \
	-I$(srcdir)/commands \
	-I$(srcdir)/core \
	-I$(srcdir)/dialogs \
	-I$(srcdir)/dockers \
	-I$(srcdir)/render \
	-I$(srcdir)/tools \
	-I$(srcdir)/visitors \
	-I$(srcdir)/widgets \
	$(all_includes)

SUBDIRS = commands visitors core shapes widgets dialogs dockers render tools . plugins data pics

# Filters should link to libkarbonbase, NOT to libkarbonpart
lib_LTLIBRARIES = libkarbonbase.la
kde_module_LTLIBRARIES = libkarbonpart.la

libkarbonpart_la_SOURCES = karbon_factory.cc 	karbon_resourceserver.cc 	karbon_part.cc 	karbon_part_iface.cc 	karbon_part_iface.skel 	karbon_view.cc 	karbon_view_iface.cc 	karbon_view_iface.skel karbon_drag.cpp karbon_grid_data.cpp

libkarbonbase_la_SOURCES = \
	karbon_view_base.cc \
	karbon_part_base.cc

libkarbonbase_la_LDFLAGS = -version-info 2:0:0 -no-undefined
libkarbonbase_la_LIBADD = $(LIB_KOFFICEUI)

libkarbonpart_la_LDFLAGS = $(KDE_PLUGIN) $(LIBFREETYPE_RPATH)
libkarbonpart_la_LIBADD = $(LIB_KOFFICEUI) $(LIB_KOPAINTER) \
	dockers/libkarbondockers.la \
	widgets/libkarbonwidgets.la \
	render/libvpainter.la \
	render/xrgbrender/libkarbonxrgbrender.la \
	tools/libkarbontools.la \
	core/libkarboncore.la \
	shapes/libkarbonshapes.la \
	commands/libkarboncommands.la \
	visitors/libkarbonvisitors.la \
	dialogs/libkarbondialogs.la \
	libkarbonbase.la \
	$(LIBART_LIBS) \
	$(LIBFONTCONFIG_LIBS) \
	$(LIBFREETYPE_LIBS)

METASOURCES = AUTO

bin_PROGRAMS =
kdeinit_LTLIBRARIES = karbon.la
karbon_la_SOURCES = main.cc
karbon_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
karbon_la_LIBADD = $(LIB_KOFFICECORE)

noinst_HEADERS = \
	karbon_factory.h \
	karbon_part_base.h \
	karbon_part.h \
	karbon_part_iface.h \
	karbon_view_base.h \
	karbon_view.h \
	karbon_drag.h

messages: rc.cpp
	$(EXTRACTRC) data/*.rc >> rc.cpp
	$(XGETTEXT) rc.cpp *.cc */*.cc tools/*.h plugins/*/*.cc -o $(podir)/karbon.pot
