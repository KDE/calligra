####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libmswriteimport_la_LDFLAGS	= -module -avoid-version -no-undefined
libmswriteimport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = libmswriteimport.la

libmswriteimport_la_SOURCES = mswriteimport.cc mswriteimport_factory.cc \
mswritelib.cc

noinst_HEADERS = mswriteimport.h mswriteimport_factory.h \
mswritelib.h
METASOURCES = AUTO

service_DATA = kword_mswrite_import.desktop
servicedir = $(kde_servicesdir)

kdemime_DATA = x-mswrite.desktop
kdemimedir = $(kde_mimedir)/application
