kde_module_LTLIBRARIES = libkritajpegimport.la libkritajpegexport.la

libkritajpegexport_la_LDFLAGS = $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -module -avoid-version -no-undefined
libkritajpegexport_la_LIBADD = \
	$(KOFFICE_LIBS) \
	$(top_builddir)/krita/libkritacommon.la \
	-ljpeg

libkritajpegimport_la_LDFLAGS = $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -module -avoid-version -no-undefined
libkritajpegimport_la_LIBADD = \
	$(KOFFICE_LIBS) \
	$(top_builddir)/krita/libkritacommon.la \
	-ljpeg

INCLUDES= \
	-I$(srcdir) \
	 $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/krita \
	-I$(top_srcdir)/krita/core \
	-I$(top_srcdir)/krita/sdk \
	-I$(top_srcdir)/krita/core/tiles \
	-I$(top_srcdir)/krita/kritacolor \
	-I$(top_srcdir)/krita/ui \
        $(KOFFICE_INCLUDES) -I$(interfacedir) \
        $(KOPAINTER_INCLUDES) \
	$(all_includes) 

service_DATA = krita_jpeg_import.desktop krita_jpeg_export.desktop
servicedir = $(kde_servicesdir)

kdelnk_DATA = krita_jpeg.desktop
kdelnkdir = $(kde_appsdir)/Office

libkritajpegimport_la_SOURCES = kis_jpeg_import.cc kis_jpeg_converter.cc
libkritajpegexport_la_SOURCES = kis_wdg_options_jpeg.ui kis_jpeg_export.cc kis_jpeg_converter.cc

METASOURCES = AUTO


KDE_CXXFLAGS = $(USE_EXCEPTIONS)
