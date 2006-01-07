INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/kivio/kiviopart \
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk \
	-I$(top_srcdir)/kivio/kiviopart/config \
        -I$(top_builddir)/kivio/kiviopart/config \
	$(all_includes)

METASOURCES = AUTO

kde_module_LTLIBRARIES = libkivioimageexport.la

libkivioimageexport_la_SOURCES = kivio_imageexport.cpp \
	kivio_imageexportwidget.ui kivio_imageexportdialog.cpp
libkivioimageexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
libkivioimageexport_la_LIBADD = $(top_builddir)/kivio/kiviopart/libkiviocommon.la $(KOFFICE_LIBS)

service_DATA = kivio_image_export.desktop
servicedir = $(kde_servicesdir)
