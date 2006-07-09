####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/kchart \
	-I$(top_srcdir)/kchart/kdchart \
	-I$(top_srcdir)/interfaces \
	-I$(top_srcdir)/filters/libdialogfilter \
	-I$(top_srcdir)/filters/kchart/libimageexport \
	$(all_includes)
####### Files

kde_module_LTLIBRARIES = libkchartmngexport.la

libkchartmngexport_la_SOURCES = mngexport.cpp
libkchartmngexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
libkchartmngexport_la_LIBADD = ../../../kchart/libkchartcommon.la ../libimageexport/libkchartimageexport.la ../../../filters/libdialogfilter/libdialogfilter.la $(KOFFICE_LIBS)
noinst_HEADERS = \
	mngexport.h

METASOURCES = AUTO

service_DATA = kchart_mng_export.desktop
servicedir = $(kde_servicesdir)

