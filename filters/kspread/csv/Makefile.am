####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libcsvfilter_la_LDFLAGS	= -module -version-info 1:0:0
#libcsvfilter_la_LIBADD = $(LIB_KOFFICECORE)

####### Files

lib_LTLIBRARIES = libcsvfilter.la

libcsvfilter_la_SOURCES = csvfilter.cc xmltree.cc csvfilter_factory.cc

noinst_HEADERS = csvfilter.h xmltree.h csvfilter_factory.h

METASOURCES = AUTO

service_DATA = kspread_csv_import.desktop
servicedir = $(kde_servicesdir)

data_DATA = x-csv.desktop
datadir = $(kde_mimedir)/text
