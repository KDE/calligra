INCLUDES = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) \
	-I$(srcdir) \
	-I$(srcdir)/commands \
	-I$(srcdir)/core \
	-I$(srcdir)/dialogs \
	-I$(srcdir)/render \
	-I$(srcdir)/tools \
	-I$(srcdir)/visitors \
	-I$(srcdir)/widgets \
	$(all_includes)

SUBDIRS = core shapes widgets dialogs commands tools render visitors . data pics

bin_PROGRAMS = karbon

lib_LTLIBRARIES = karbon.la
kde_module_LTLIBRARIES = libkarbonpart.la

libkarbonpart_la_SOURCES = \
	karbon_factory.cc \
	karbon_part.cc \
	karbon_part_iface.cc \
	karbon_part_iface.skel \
	karbon_view.cc \
	karbon_view_iface.cc \
	karbon_view_iface.skel

libkarbonpart_la_LDFLAGS = $(KDE_PLUGIN)
libkarbonpart_la_LIBADD = $(LIB_KOFFICEUI) $(LIB_KOTEXT) \
	tools/libkarbontools.la \
	dialogs/libkarbondialogs.la \
	widgets/libkarbonwidgets.la \
	commands/libkarboncommands.la \
	render/libvpainter.la \
	shapes/libkarbonshapes.la \
	core/libkarboncore.la \
	visitors/libkarbonvisitors.la 

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
	karbon_part.h \
	karbon_part_iface.h \
	karbon_view.h

messages: rc.cpp
	$(EXTRACTRC) data/*.rc >> rc.cpp
	$(XGETTEXT) rc.cpp *.cc */*.cc */*.cpp -o $(podir)/karbon.pot
