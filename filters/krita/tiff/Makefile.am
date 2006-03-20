kde_module_LTLIBRARIES = libkritatiffimport.la libkritatiffexport.la

libkritatiffexport_la_LDFLAGS = -avoid-version -module -no-undefined \
	$(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
libkritatiffexport_la_LIBADD = $(top_builddir)/krita/libkritacommon.la \
	libkritatiffconverter.la $(KOFFICE_LIBS) -ltiff

libkritatiffimport_la_LDFLAGS = -avoid-version -module -no-undefined \
	$(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
libkritatiffimport_la_LIBADD = $(top_builddir)/krita/libkritacommon.la \
	libkritatiffconverter.la $(KOFFICE_LIBS) -ltiff

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


servicedir = $(kde_servicesdir)


kdelnkdir = $(kde_appsdir)/Office




METASOURCES = AUTO


KDE_CXXFLAGS = $(USE_EXCEPTIONS)
libkritatiffimport_la_SOURCES = kis_tiff_import.cc
libkritatiffexport_la_SOURCES = kis_tiff_export.cc kis_wdg_options_tiff.ui \
	kis_dlg_options_tiff.cpp
service_DATA = krita_tiff_export.desktop krita_tiff_import.desktop
kdelnk_DATA = krita_tiff.desktop
noinst_HEADERS = kis_dlg_options_tiff.h kis_tiff_writer_visitor.h \
		kis_ycbcr_u8_colorspace.h kis_tiff_ycbcr_reader.h
libkritatiffconverter_la_LDFLAGS = -no-undefined $(all_libraries)
noinst_LTLIBRARIES = libkritatiffconverter.la
libkritatiffconverter_la_SOURCES = kis_tiff_converter.cc kis_tiff_stream.cc \
					kis_tiff_writer_visitor.cpp kis_tiff_reader.cc kis_ycbcr_u8_colorspace.cpp kis_tiff_ycbcr_reader.cc \
	kis_ycbcr_u16_colorspace.cpp
