SUBDIRS = xpdf

INCLUDES= -I$(srcdir) $(all_includes) -I$(srcdir)/xpdf/goo -I$(srcdir)/xpdf/xpdf

kde_module_LTLIBRARIES = libpdfimport.la

libpdfimport_la_SOURCES = misc.cpp FilterPage.cpp FilterDevice.cpp \
                          pdfdocument.cpp dialog.cpp pdfimport.cpp
libpdfimport_la_LDFLAGS	= -module $(KDE_PLUGIN)
libpdfimport_la_LIBADD = $(KDE_RPATH) $(all_libraries) $(LIBKOFFICE) ./xpdf/xpdf/libxpdf.la ./xpdf/goo/libgoo.la

METASOURCES = dialog.moc pdfimport.moc

service_DATA = kword_pdf_import.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kwordpdfimport.pot
