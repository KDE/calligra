AM_CPPFLAGS= \
	-I$(srcdir) \
	-I$(top_srcdir)/krita \
	-I$(top_srcdir)/krita/core \
	-I$(top_srcdir)/krita/sdk \
	-I$(top_srcdir)/krita/kritacolor \
	-I$(top_srcdir)/krita/ui \
	-I$(top_srcdir)/filters/krita/magick \
	$(KOFFICE_INCLUDES) \
	$(all_includes) 

kde_module_LTLIBRARIES = libkrita_raw_import.la 

libkrita_raw_import_la_LDFLAGS = $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -module -avoid-version -no-undefined
libkrita_raw_import_la_LIBADD = \
	$(KOFFICE_LIBS) \
	$(raw_LIBS) \
	$(top_builddir)/krita/libkritacommon.la 


service_DATA = krita_raw_import.desktop
servicedir = $(kde_servicesdir)

kdelnk_DATA = krita_raw.desktop
kdelnkdir = $(kde_appsdir)/Office

libkrita_raw_import_la_SOURCES = kis_raw_import.cpp wdgrawimport.ui

METASOURCES = AUTO

KDE_CXXFLAGS = $(USE_EXCEPTIONS)


