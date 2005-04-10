####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/kchart \
	-I$(top_srcdir)/kchart/kdchart \
	$(all_includes)

####### Files
# Obviously you have to change "foo" (and maybe "import") to
# reflect the name of your filter. If you have more files
# than just svgexport.cc please add them to the _SOURCES line.

kde_module_LTLIBRARIES = libkchartsvgexport.la

libkchartsvgexport_la_SOURCES = svgexport.cc
libkchartsvgexport_la_LDFLAGS	= -module $(KDE_PLUGIN)
libkchartsvgexport_la_LIBADD = $(KOFFICE_LIBS) \
	../../../kchart/libkchartpart.la \
	../../../kchart/kdchart/libkdchart.la
noinst_HEADERS = \
	svgexport.h

METASOURCES = AUTO

service_DATA = kchart_svg_export.desktop
servicedir = $(kde_servicesdir)

# Note: If your filter exports or exports some special file
# which KDE doesn't have a mimetype for, yet, you'll have to
# create a mimetype and install it using those two lines.
# In case of doubt please ask koffice@mail.kde.org or
# koffice-devel@mail.kde.org. Thanks.
# data_DATA = x-png.desktop
# datadir = $(kde_mimedir)/text
