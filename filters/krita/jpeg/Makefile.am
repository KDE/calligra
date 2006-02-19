noinst_LTLIBRARIES = libkritaconverter.la
kde_module_LTLIBRARIES = libkritajpegimport.la libkritajpegexport.la

libkritaconverter_la_LDFLAGS = $(all_libraries)
libkritaconverter_la_SOURCES = kis_jpeg_converter.cc iccjpeg.c
libkritaconverter_la_LIBADD = $(top_builddir)/filters/krita/libkisexif/libkisexif.la

libkritajpegexport_la_LDFLAGS = -avoid-version -module -no-undefined \
	$(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
libkritajpegexport_la_LIBADD = $(top_builddir)/krita/libkritacommon.la \
	libkritaconverter.la $(KOFFICE_LIBS) -ljpeg -lexif

libkritajpegimport_la_LDFLAGS = -avoid-version -module -no-undefined \
	$(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
libkritajpegimport_la_LIBADD = $(top_builddir)/krita/libkritacommon.la \
	libkritaconverter.la $(KOFFICE_LIBS) -ljpeg -lexif

INCLUDES= \
	-I$(srcdir) \
	 $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/krita \
	-I$(top_srcdir)/krita/core \
	-I$(top_srcdir)/krita/sdk \
	-I$(top_srcdir)/krita/core/tiles \
	-I$(top_srcdir)/krita/kritacolor \
	-I$(top_srcdir)/krita/ui \
	-I$(top_srcdir)/filters/krita/libkisexif \
        $(KOFFICE_INCLUDES) -I$(interfacedir) \
        $(KOPAINTER_INCLUDES) \
	$(all_includes) 

service_DATA = krita_jpeg_import.desktop krita_jpeg_export.desktop
servicedir = $(kde_servicesdir)

kdelnk_DATA = krita_jpeg.desktop
kdelnkdir = $(kde_appsdir)/Office

libkritajpegimport_la_SOURCES = kis_jpeg_import.cc
libkritajpegexport_la_SOURCES = kis_wdg_options_jpeg.ui kis_jpeg_export.cc

METASOURCES = AUTO


KDE_CXXFLAGS = $(USE_EXCEPTIONS)

