####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libmswriteimport_la_LDFLAGS = -module $(KDE_PLUGIN)
libmswriteimport_la_LIBADD = $(LIB_KOFFICECORE)

####### Files

kde_module_LTLIBRARIES = libmswriteimport.la

libmswriteimport_la_SOURCES = mswriteimport.cc mswritelib.cc

noinst_HEADERS = mswriteimport.h mswritelib.h
METASOURCES = AUTO

service_DATA = kword_mswrite_import.desktop
servicedir = $(kde_servicesdir)

kdemime_DATA = x-mswrite.desktop
kdemimedir = $(kde_mimedir)/application
