####### General stuff

INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
-I$(top_srcdir)/lib/kscript $(all_includes)

####### Files

lib_LTLIBRARIES = libcsvfilter.la libcsvfilterdia.la \
libcsvexport.la libcsvexport_test.la

libcsvfilter_la_SOURCES = csvfilter.cc xmltree.cc csvfilter_factory.cc
libcsvfilter_la_LDFLAGS	= -module -avoid-version -no-undefined
libcsvfilter_la_LIBADD  = $(KOFFICE_LIBS)

libcsvfilterdia_la_SOURCES = csvfilterdia.cc csvfilterdia_factory.cc
libcsvfilterdia_la_LDFLAGS = -module -avoid-version -no-undefined
libcsvfilterdia_la_LIBADD  = $(KOFFICE_LIBS)

libcsvexport_la_SOURCES = csvexport.cc csvexport_factory.cc
libcsvexport_la_LDFLAGS = -module -avoid-version -no-undefined
libcsvexport_la_LIBADD  = $(KOFFICE_LIBS)

libcsvexport_test_la_SOURCES = csvexport_test.cc csvexport_factory_test.cc
libcsvexport_test_la_LDFLAGS = -module -avoid-version -no-undefined
libcsvexport_test_la_LIBADD  = $(KOFFICE_LIBS) \
../../../kspread/libkspread.la

noinst_HEADERS = csvfilter.h xmltree.h csvfilter_factory.h \
csvfilterdia.h csvfilterdia_factory.h csvexport.h csvexport_factory.h \
csvexport_test.h csvexport_factory_test.h

METASOURCES = AUTO

service_DATA = kspread_csv_import.desktop kspread_csv_import_dia.desktop \
kspread_csv_export_dia.desktop kspread_csv_export_test.desktop
#kspread_csv_export.desktop

servicedir = $(kde_servicesdir)

data_DATA = x-csv.desktop
datadir = $(kde_mimedir)/text
