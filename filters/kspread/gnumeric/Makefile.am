####### General stuff

INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
-I$(top_srcdir)/lib/kscript -I$(top_srcdir)/lib/koscript $(all_includes)

####### Files
kde_module_LTLIBRARIES = libgnumericexport.la libgnumericimport.la 

libgnumericexport_la_SOURCES = gnumericexport.cc gnumericexport_factory.cc
libgnumericexport_la_LDFLAGS = -module -avoid-version -no-undefined
libgnumericexport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadpart.la

libgnumericimport_la_SOURCES = gnumericimport.cc gnumericimport_factory.cc
libgnumericimport_la_LDFLAGS = -module -avoid-version -no-undefined
libgnumericimport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadpart.la

METASOURCES = AUTO

service_DATA = kspread_gnumeric_export.desktop kspread_gnumeric_import.desktop

servicedir = $(kde_servicesdir)

data_DATA = x-gnumeric.desktop
datadir = $(kde_mimedir)/text
