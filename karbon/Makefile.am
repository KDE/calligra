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

SUBDIRS = core widgets dialogs commands tools render visitors . data pics

bin_PROGRAMS = karbon

lib_LTLIBRARIES = \
	karbon.la \
	libkarbonpart.la

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
	shapes/libkarbonshapes.la \
	tools/libkarbontools.la \
	commands/libkarboncommands.la \
	visitors/libkarbonvisitors.la \
	render/libvpainter.la \
	core/libkarboncore.la \
	dialogs/libkarbondialogs.la \
	widgets/libkarbonwidgets.la

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

