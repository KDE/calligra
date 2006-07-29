####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/kchart \
	-I$(top_srcdir)/kchart/kdchart \
	-I$(top_srcdir)/interfaces \
	-I$(top_srcdir)/filters/libdialogfilter \
	-I$(top_srcdir)/filters/kchart/libimageexport \
	$(all_includes)

####### Files

kde_module_LTLIBRARIES = libkchartxbmexport.la

libkchartxbmexport_la_SOURCES = xbmexport.cpp
libkchartxbmexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
libkchartxbmexport_la_LIBADD = ../../../kchart/libkchartcommon.la ../libimageexport/libkchartimageexport.la ../../../filters/libdialogfilter/libdialogfilter.la $(KOFFICE_LIBS)
noinst_HEADERS = \
	xbmexport.h

METASOURCES = AUTO

service_DATA = kchart_xbm_export.desktop
servicedir = $(kde_servicesdir)

