AM_CPPFLAGS= \
	-I$(srcdir) \
	-I$(top_srcdir)/krita \
	-I$(top_srcdir)/krita/core \
	-I$(top_srcdir)/krita/sdk \
	-I$(top_srcdir)/krita/core/tiles \
	-I$(top_srcdir)/krita/kritacolor \
	-I$(top_srcdir)/krita/ui \
    $(KOFFICE_INCLUDES) \
	$(all_includes) 

kde_module_LTLIBRARIES = libkrita_raw_import.la 

libkrita_raw_import_la_LDFLAGS = $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -module -avoid-version -no-undefined
libkrita_raw_import_la_LIBADD = \
	$(KOFFICE_LIBS) \
	$(raw_LIBS) \
	$(top_builddir)/krita/libkritacommon.la \
	$(top_builddir)/krita/colorspaces/rgb_f32/libkrita_rgb_f32.la \
	$(top_builddir)/krita/colorspaces/rgb_f16half/libkrita_rgb_f16half.la


service_DATA = krita_raw_import.desktop
servicedir = $(kde_servicesdir)

kdelnk_DATA = krita_raw.desktop
kdelnkdir = $(kde_appsdir)/Office

libkrita_raw_import_la_SOURCES = kis_raw_import.cpp wdgrawimport.ui

METASOURCES = AUTO

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

mimetypeimagedata_DATA = x-raw.desktop
mimetypeimagedatadir = $(kde_mimedir)/image
EXTRA_DIST = $(mimetypeimagedata_DATA)

