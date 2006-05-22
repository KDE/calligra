INCLUDES = \
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
        $(all_includes)  $(POPPLER_CFLAGS)

servicedir = $(kde_servicesdir)

kdelnkdir = $(kde_appsdir)/Office

METASOURCES = AUTO
kde_module_LTLIBRARIES = libkritapdfimport.la
libkritapdfimport_la_LDFLAGS = -avoid-version -module -no-undefined  $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
libkritapdfimport_la_LIBADD = $(top_builddir)/krita/libkritacommon.la \
	-lpoppler-qt $(KOFFICE_LIBS) $(POPPLER_LIBS)

noinst_HEADERS = kis_pdf_import.h kis_pdf_import_widget.h
libkritapdfimport_la_SOURCES = kis_pdf_import.cpp pdfimportwidgetbase.ui \
	kis_pdf_import_widget.cpp
kde_services_DATA = krita_pdf_import.desktop
kdelnk_DATA = krita_pdf.desktop
