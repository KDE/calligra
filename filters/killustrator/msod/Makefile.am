####### General stuff

INCLUDES= -I$(srcdir) $(KWMF_INCLUDES) $(all_includes)
libmsodimport_la_LDFLAGS	= -module -avoid-version -no-undefined
libmsodimport_la_LIBADD = $(KOFFICE_LIBS) $(LIB_KWMF)

####### Files

lib_LTLIBRARIES = libmsodimport.la

libmsodimport_la_SOURCES = msodimport.cc msodimport_factory.cc msod.cc

noinst_HEADERS = msodimport.h msodimport_factory.h msod.h

METASOURCES = AUTO

service_DATA = killustrator_msod_import.desktop
servicedir = $(kde_servicesdir)
data_DATA = x-msod.desktop
datadir = $(kde_mimedir)/image
