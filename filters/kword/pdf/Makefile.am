SUBDIRS = xpdf

INCLUDES= -I$(srcdir) -I$(srcdir)/xpdf/goo -I$(srcdir)/xpdf/xpdf $(all_includes)

kde_module_LTLIBRARIES = libpdfimport.la

libpdfimport_la_SOURCES = misc.cpp transform.cpp data.cpp fstring.cpp \
                          FilterPage.cpp FilterDevice.cpp \
                          pdfdocument.cpp dialog.cpp pdfimport.cpp
libpdfimport_la_LDFLAGS	= $(KDE_PLUGIN)
libpdfimport_la_LIBADD = $(KDE_RPATH) $(all_libraries) $(LIBKOFFICE) ./xpdf/xpdf/libxpdf.la ./xpdf/goo/libgoo.la

METASOURCES = dialog.moc pdfimport.moc

service_DATA = kword_pdf_import.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kwordpdfimport.pot
