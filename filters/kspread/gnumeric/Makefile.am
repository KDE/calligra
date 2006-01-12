####### General stuff

INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
	$(KOTEXT_INCLUDES) $(all_includes)

####### Files
kde_module_LTLIBRARIES = libgnumericexport.la libgnumericimport.la

libgnumericexport_la_SOURCES = gnumericexport.cc
libgnumericexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libgnumericexport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la

libgnumericimport_la_SOURCES = gnumericimport.cc
libgnumericimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libgnumericimport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la

METASOURCES = AUTO

service_DATA = kspread_gnumeric_export.desktop kspread_gnumeric_import.desktop

servicedir = $(kde_servicesdir)
