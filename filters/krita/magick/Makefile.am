INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) -I$(top_srcdir)/krita -I$(top_srcdir)/krita/core $(all_includes) 

kde_module_LTLIBRARIES = libmagickimport.la

libmagickimport_la_SOURCES = magickimport.cpp
libmagickimport_la_LDFLAGS = -module $(KDE_PLUGIN)
libmagickimport_la_LIBADD = $(KOFFICE_LIBS) $(top_builddir)/krita/libkritacommon.la

METASOURCES = AUTO

service_DATA = krita_magick_import.desktop
servicedir = $(kde_servicesdir)

