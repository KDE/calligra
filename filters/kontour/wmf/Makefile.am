####### General stuff

INCLUDES= -I$(srcdir) $(KWMF_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes)
libwmfimport_la_LDFLAGS	= -module -avoid-version -no-undefined
libwmfimport_la_LIBADD = $(KOFFICE_LIBS) $(LIB_KWMF)

####### Files

kde_module_LTLIBRARIES = libwmfimport.la

libwmfimport_la_SOURCES = wmfimport.cc wmfimport_factory.cc

noinst_HEADERS = wmfimport.h wmfimport_factory.h

METASOURCES = AUTO

service_DATA = killustrator_wmf_import.desktop
servicedir = $(kde_servicesdir)
data_DATA = x-wmf.desktop
datadir = $(kde_mimedir)/image
