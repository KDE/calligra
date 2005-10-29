####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/kchart \
	-I$(top_srcdir)/kchart/kdchart \
	-I$(top_srcdir)/interfaces \
	-I$(top_srcdir)/filters/libdialogfilter \
	-I$(top_srcdir)/filters/kchart/libimageexport \
	$(all_includes)

####### Files

kde_module_LTLIBRARIES = libkchartpngexport.la

libkchartpngexport_la_SOURCES = pngexport.cpp
libkchartpngexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
libkchartpngexport_la_LIBADD = ../../../kchart/libkchartcommon.la ../libimageexport/libkchartimageexport.la ../../../filters/libdialogfilter/libdialogfilter.la $(KOFFICE_LIBS)
noinst_HEADERS = \
	pngexport.h 

METASOURCES = AUTO

service_DATA = kchart_png_export.desktop
servicedir = $(kde_servicesdir)

# Note: If your filter exports or exports some special file
# which KDE doesn't have a mimetype for, yet, you'll have to
# create a mimetype and install it using those two lines.
# In case of doubt please ask koffice@kde.org or
# koffice-devel@kde.org. Thanks.
# data_DATA = x-png.desktop
# datadir = $(kde_mimedir)/text
