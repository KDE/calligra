####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libcsvfilter_la_LDFLAGS	= -module -avoid-version
libcsvfilterdia_la_LDFLAGS = -module -avoid-version

####### Files

lib_LTLIBRARIES = libcsvfilter.la libcsvfilterdia.la

libcsvfilter_la_SOURCES = csvfilter.cc xmltree.cc csvfilter_factory.cc

libcsvfilterdia_la_SOURCES = csvfilterdia.cc csvfilterdia_factory.cc

noinst_HEADERS = csvfilter.h xmltree.h csvfilter_factory.h \
csvfilterdia.h csvfilterdia_factory.h

METASOURCES = AUTO

service_DATA = kspread_csv_import.desktop kspread_csv_dia.desktop
servicedir = $(kde_servicesdir)

data_DATA = x-csv.desktop
datadir = $(kde_mimedir)/text
