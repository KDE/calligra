INCLUDES = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) \
	-I$(srcdir) \
	-I$(srcdir)/core \
	-I$(srcdir)/visitors \
	-I$(srcdir)/tools \
	-I$(srcdir)/commands \
	-I$(srcdir)/ui \
	-I$(srcdir)/render \
	$(all_includes)

SUBDIRS = core visitors ui commands tools render . data pics

bin_PROGRAMS = karbon
lib_LTLIBRARIES = \
	karbon.la \
	libkarbonpart.la

libkarbonpart_la_SOURCES = \
	karbon_part.cc \
	karbon_view.cc \
	karbon_factory.cc karbon_view_iface.skel karbon_view_iface.cc \
	karbon_dlg_config.cc karbon_part_iface.skel karbon_part_iface.cc 

libkarbonpart_la_LDFLAGS = $(KDE_PLUGIN)
libkarbonpart_la_LIBADD = $(LIB_KOFFICEUI) $(LIB_KOTEXT) \
	tools/libkarbontools.la \
	commands/libkarboncommands.la \
	core/libkarboncore.la \
	visitors/libkarbonvisitors.la \
	render/libvpainter.la

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
	karbon_part.h \
	karbon_view.h \
	karbon_factory.h \
	karbon_dlg_config.h \
	karbon_part_iface.h
