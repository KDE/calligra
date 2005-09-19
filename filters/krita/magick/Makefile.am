kde_module_LTLIBRARIES = libkritamagickimport.la libkritamagickexport.la

libkritamagickexport_la_LDFLAGS = $(KDE_PLUGIN)
libkritamagickexport_la_LIBADD = \
	$(KOFFICE_LIBS) \
	$(top_builddir)/krita/libkritacommon.la

libkritamagickimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libkritamagickimport_la_LIBADD = \
	$(KOFFICE_LIBS) \
	$(top_builddir)/krita/libkritacommon.la

INCLUDES= \
	-I$(srcdir) \
	 $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/krita \
	-I$(top_srcdir)/krita/core \
	-I$(top_srcdir)/krita/ui \
	$(all_includes) 

service_DATA = krita_magick_import.desktop krita_magick_export.desktop
servicedir = $(kde_servicesdir)

kdelnk_DATA = krita_magick.desktop
kdelnkdir = $(kde_appsdir)/Office

libkritamagickimport_la_SOURCES = magickimport.cpp
libkritamagickexport_la_SOURCES = magickexport.cpp

METASOURCES = AUTO


