kde_module_LTLIBRARIES = libkritapngimport.la libkritapngexport.la

libkritapngexport_la_LDFLAGS = $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -module -avoid-version -no-undefined
libkritapngexport_la_LIBADD = \
	$(KOFFICE_LIBS) \
	$(top_builddir)/krita/libkritacommon.la \
	-lpng

libkritapngimport_la_LDFLAGS = $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -module -avoid-version -no-undefined
libkritapngimport_la_LIBADD = \
	$(KOFFICE_LIBS) \
	$(top_builddir)/krita/libkritacommon.la \
	-lpng

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

service_DATA = krita_png_import.desktop krita_png_export.desktop
servicedir = $(kde_servicesdir)

kdelnk_DATA = krita_png.desktop
kdelnkdir = $(kde_appsdir)/Office

libkritapngimport_la_SOURCES = kis_png_import.cc kis_png_converter.cc
libkritapngexport_la_SOURCES = kis_wdg_options_png.ui kis_png_export.cc kis_png_converter.cc

METASOURCES = AUTO


KDE_CXXFLAGS = $(USE_EXCEPTIONS)
