####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
        -I$(top_srcdir)/kpresenter \
        -I$(top_srcdir)/lib/kotext \
	$(all_includes)

####### Files
kde_module_LTLIBRARIES = libkpresentersvgexport.la

libkpresentersvgexport_la_SOURCES = svgexport.cc
libkpresentersvgexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
libkpresentersvgexport_la_LIBADD = ../../../kpresenter/libkpresenterprivate.la $(KOFFICE_LIBS)
noinst_HEADERS = \
	svgexport.h

METASOURCES = AUTO

service_DATA = kpresenter_svg_export.desktop
servicedir = $(kde_servicesdir)

