####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/kpresenter \
        -I$(top_srcdir)/lib/kotext \	
	$(all_includes)

####### Files

kde_module_LTLIBRARIES = libkpresenterpngexport.la

libkpresenterpngexport_la_SOURCES = pngexport.cpp pngexportdia.cpp
libkpresenterpngexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
libkpresenterpngexport_la_LIBADD = ../../../kpresenter/libkpresenterpart.la $(KOFFICE_LIBS)
noinst_HEADERS = \
	pngexport.h pngexportdia.h

METASOURCES = AUTO

service_DATA = kpresenter_png_export.desktop
servicedir = $(kde_servicesdir)

