kde_module_LTLIBRARIES = libkritaxcfimport.la libkritaxcfexport.la

libkritaxcfexport_la_LDFLAGS = $(KDE_PLUGIN) $(LIBMAGICK_LDFLAGS) $(KDE_RPATH) $(LIBMAGICK_RPATH) $(all_libraries) -module -avoid-version -no-undefined
libkritaxcfexport_la_LIBADD = \
	$(KOFFICE_LIBS) \
	$(LIBMAGICK_LIBS) \
	$(top_builddir)/krita/libkritacommon.la

libkritaxcfimport_la_LDFLAGS = $(KDE_PLUGIN) $(LIBMAGICK_LDFLAGS) $(KDE_RPATH) $(LIBMAGICK_RPATH) $(all_libraries) -module -avoid-version -no-undefined
libkritaxcfimport_la_LIBADD = \
	$(KOFFICE_LIBS) \
	$(LIBMAGICK_LIBS) \
	$(top_builddir)/krita/libkritacommon.la

INCLUDES= \
	-I$(srcdir) \
	 $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/krita \
	-I$(top_srcdir)/krita/core \
	-I$(top_srcdir)/krita/sdk \
	-I$(top_srcdir)/krita/core/tool \
	-I$(top_srcdir)/krita/core/resources \
	-I$(top_srcdir)/krita/core/strategy \
	-I$(top_srcdir)/krita/core/tiles \
	-I$(top_srcdir)/krita/core/visitors \
	-I$(top_srcdir)/krita/core/color_strategy \
	-I$(top_srcdir)/krita/core/paintop \
	-I$(top_srcdir)/krita/ui \
        $(KOFFICE_INCLUDES) -I$(interfacedir) \
        $(KOPAINTER_INCLUDES) $(LIBMAGICK_CPPFLAGS) \
	$(all_includes) 

service_DATA = krita_xcf_import.desktop krita_xcf_export.desktop
servicedir = $(kde_servicesdir)

libkritaxcfimport_la_SOURCES = xcfimport.cpp kis_xcf_converter.cc
libkritaxcfexport_la_SOURCES = xcfexport.cpp kis_xcf_converter.cc

METASOURCES = AUTO


KDE_CXXFLAGS = $(USE_EXCEPTIONS)
