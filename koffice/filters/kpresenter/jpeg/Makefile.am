####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/kpresenter \
        -I$(top_srcdir)/lib/kotext \
	-I$(top_srcdir)/filters/libdialogfilter \
	-I$(top_srcdir)/filters/kpresenter/libimageexport \
	$(all_includes)

####### Files

kde_module_LTLIBRARIES = libkpresenterjpegexport.la

libkpresenterjpegexport_la_SOURCES = jpegexport.cpp
libkpresenterjpegexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
libkpresenterjpegexport_la_LIBADD = ../../../kpresenter/libkpresenterprivate.la ../../../filters/libdialogfilter/libdialogfilter.la ../libimageexport/libkpresenterimageexport.la $(KOFFICE_LIBS)
noinst_HEADERS = \
	jpegexport.h

METASOURCES = AUTO

service_DATA = kpresenter_jpeg_export.desktop
servicedir = $(kde_servicesdir)

