####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/kpresenter \
        -I$(top_srcdir)/lib/kotext \
	-I$(top_srcdir)/filters/libdialogfilter \
	-I$(top_srcdir)/filters/kpresenter/libimageexport \
	$(all_includes)

####### Files

kde_module_LTLIBRARIES = libkpresenterpngexport.la

libkpresenterpngexport_la_SOURCES = pngexport.cpp
libkpresenterpngexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
libkpresenterpngexport_la_LIBADD = ../../../kpresenter/libkpresenterprivate.la ../../../filters/libdialogfilter/libdialogfilter.la ../libimageexport/libkpresenterimageexport.la $(KOFFICE_LIBS)
noinst_HEADERS = \
	pngexport.h

METASOURCES = AUTO

service_DATA = kpresenter_png_export.desktop
servicedir = $(kde_servicesdir)

