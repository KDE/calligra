####### General stuff

INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
-I$(top_srcdir)/lib/kscript -I$(top_srcdir)/lib/koscript $(all_includes)

####### Files

kde_module_LTLIBRARIES = libcsvimport.la libcsvfilterdia.la \
libcsvexport.la

libcsvimport_la_SOURCES = csvimport.cc xmltree.cc csvimport_factory.cc
libcsvimport_la_LDFLAGS	= -module -avoid-version -no-undefined
libcsvimport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadpart.la

libcsvfilterdia_la_SOURCES = csvfilterdia.cc csvfilterdia_factory.cc
libcsvfilterdia_la_LDFLAGS = -module -avoid-version -no-undefined
libcsvfilterdia_la_LIBADD  = $(KOFFICE_LIBS)

libcsvexport_la_SOURCES = csvexport.cc csvexport_factory.cc
libcsvexport_la_LDFLAGS = -module -avoid-version -no-undefined
libcsvexport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadpart.la

METASOURCES = AUTO

service_DATA = kspread_csv_import.desktop kspread_csv_import_dia.desktop \
kspread_csv_export_dia.desktop kspread_csv_export.desktop

servicedir = $(kde_servicesdir)

data_DATA = x-csv.desktop
datadir = $(kde_mimedir)/text
