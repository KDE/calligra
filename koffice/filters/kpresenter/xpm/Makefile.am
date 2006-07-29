####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/kpresenter \
        -I$(top_srcdir)/lib/kotext \
	-I$(top_srcdir)/filters/libdialogfilter \
	-I$(top_srcdir)/filters/kpresenter/libimageexport \
	$(all_includes)

####### Files

kde_module_LTLIBRARIES = libkpresenterxpmexport.la

libkpresenterxpmexport_la_SOURCES = xpmexport.cpp
libkpresenterxpmexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
libkpresenterxpmexport_la_LIBADD = ../../../kpresenter/libkpresenterprivate.la ../../../filters/libdialogfilter/libdialogfilter.la ../libimageexport/libkpresenterimageexport.la $(KOFFICE_LIBS)
noinst_HEADERS = \
	xpmexport.h

METASOURCES = AUTO

service_DATA = kpresenter_xpm_export.desktop
servicedir = $(kde_servicesdir)

