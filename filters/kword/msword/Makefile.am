####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(LIBWV2_CFLAGS) $(all_includes)

####### Files

kde_module_LTLIBRARIES = libmswordimport.la

libmswordimport_la_SOURCES = mswordimport.cpp document.cpp conversion.cpp \
	texthandler.cpp tablehandler.cpp graphicshandler.cpp
libmswordimport_la_LDFLAGS = -module $(KDE_PLUGIN)
libmswordimport_la_LIBADD = $(KOFFICE_LIBS) $(LIBWV2_LIBS)

METASOURCES = AUTO

service_DATA = kword_msword_import.desktop
servicedir = $(kde_servicesdir)
