KDE_OPTIONS = nofinal

SUBDIRS = xpdf

INCLUDES= $(KOFFICE_INCLUDES) -I$(srcdir) -I$(srcdir)/xpdf/goo -I$(srcdir)/xpdf/xpdf $(all_includes)

kde_module_LTLIBRARIES = libpdfimport.la

libpdfimport_la_SOURCES = misc.cpp transform.cpp data.cpp fstring.cpp \
                          FilterPage.cpp FilterDevice.cpp \
                          pdfdocument.cpp dialog.cpp pdfimport.cpp
libpdfimport_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libpdfimport_la_LIBADD = $(LIB_KOFFICECORE) $(LIBPAPER) ./xpdf/xpdf/libxpdf.la ./xpdf/goo/libgoo.la

METASOURCES = dialog.moc pdfimport.moc

service_DATA = kword_pdf_import.desktop
servicedir = $(kde_servicesdir)
