AM_CPPFLAGS= \
	-I$(srcdir) \
	-I$(top_srcdir)/krita \
	-I$(top_srcdir)/krita/core \
	-I$(top_srcdir)/krita/sdk \
	-I$(top_srcdir)/krita/core/resources \
	-I$(top_srcdir)/krita/core/strategy \
	-I$(top_srcdir)/krita/core/tiles \
	-I$(top_srcdir)/krita/core/visitors \
	-I$(top_srcdir)/krita/core/color_strategy \
	-I$(top_srcdir)/krita/core/paintop \
	-I$(top_srcdir)/krita/ui \
	-I$(top_srcdir)/krita/modules/rgb_f32 \
        $(KOFFICE_INCLUDES) \
	-I$(interfacedir) \
        $(OPENEXR_CFLAGS) \
	$(all_includes) 

kde_module_LTLIBRARIES = libkrita_openexr_import.la libkrita_openexr_export.la

libkrita_openexr_export_la_LDFLAGS = $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -module -avoid-version -no-undefined
libkrita_openexr_export_la_LIBADD = \
	$(KOFFICE_LIBS) \
	$(OPENEXR_LIBS) \
	$(top_builddir)/krita/libkritacommon.la \
	$(top_builddir)/krita/modules/rgb_f32/libkrita_rgb_f32.la


libkrita_openexr_import_la_LDFLAGS = $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -module -avoid-version -no-undefined
libkrita_openexr_import_la_LIBADD = \
	$(KOFFICE_LIBS) \
	$(OPENEXR_LIBS) \
	$(top_builddir)/krita/libkritacommon.la \
	$(top_builddir)/krita/modules/rgb_f32/libkrita_rgb_f32.la


service_DATA = krita_openexr_import.desktop krita_openexr_export.desktop
servicedir = $(kde_servicesdir)

libkrita_openexr_import_la_SOURCES = kis_openexr_import.cpp
libkrita_openexr_export_la_SOURCES = kis_openexr_export.cpp

METASOURCES = AUTO

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

