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

bin_PROGRAMS = karbon

lib_LTLIBRARIES = karbon.la
kde_module_LTLIBRARIES = libkarbonbase.la libkarbonpart.la

libkarbonpart_la_SOURCES = karbon_factory.cc 	karbon_resourceserver.cc 	karbon_part.cc 	karbon_part_iface.cc 	karbon_part_iface.skel 	karbon_view.cc 	karbon_view_iface.cc 	karbon_view_iface.skel karbon_drag.cpp karbon_grid_data.cpp

libkarbonbase_la_SOURCES = \
	karbon_view_base.cc \
	karbon_part_base.cc

libkarbonbase_la_LDFLAGS = $(KDE_PLUGIN)
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

libkarbonpart_la_METASOURCES = AUTO

karbon_la_SOURCES = main.cc
karbon_la_LDFLAGS = $(all_libraries) -module -avoid-version
karbon_la_LIBADD = libkarbonpart.la

karbon_SOURCES = dummy.cc
karbon_LDFLAGS = $(all_libraries) $(KDE_RPATH)
karbon_LDADD = karbon.la

dummy.cc:
	echo > dummy.cc

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
