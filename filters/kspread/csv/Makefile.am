####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

####### Files

lib_LTLIBRARIES = libcsvfilter.la libcsvfilterdia.la libcsvexport.la

libcsvfilter_la_SOURCES = csvfilter.cc xmltree.cc csvfilter_factory.cc
libcsvfilter_la_LDFLAGS	= -module -avoid-version -no-undefined
libcsvfilter_la_LIBADD  = $(KOFFICE_LIBS)

libcsvfilterdia_la_SOURCES = csvfilterdia.cc csvfilterdia_factory.cc
libcsvfilterdia_la_LDFLAGS = -module -avoid-version -no-undefined
libcsvfilterdia_la_LIBADD  = $(KOFFICE_LIBS)

libcsvexport_la_SOURCES = csvexport.cc csvexport_factory.cc
libcsvexport_la_LDFLAGS = -module -avoid-version -no-undefined
libcsvexport_la_LIBADD  = $(KOFFICE_LIBS)

noinst_HEADERS = csvfilter.h xmltree.h csvfilter_factory.h \
csvfilterdia.h csvfilterdia_factory.h csvexport.h csvexport_factory.h

METASOURCES = AUTO

service_DATA = kspread_csv_import.desktop kspread_csv_export.desktop \
kspread_csv_import_dia.desktop kspread_csv_export_dia.desktop
servicedir = $(kde_servicesdir)

data_DATA = x-csv.desktop
datadir = $(kde_mimedir)/text
