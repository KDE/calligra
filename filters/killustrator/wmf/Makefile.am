####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libwmfimport_la_LDFLAGS	= -module -avoid-version -no-undefined
libwmfimport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

lib_LTLIBRARIES = libwmfimport.la

libwmfimport_la_SOURCES = wmfimport.cc wmfimport_factory.cc wmf.cc

noinst_HEADERS = wmfimport.h wmfimport_factory.h wmf.h

METASOURCES = AUTO

service_DATA = killustrator_wmf_import.desktop
servicedir = $(kde_servicesdir)
data_DATA = x-wmf.desktop
datadir = $(kde_mimedir)/image
