####### General stuff

INCLUDES= -I$(srcdir) -I$(srcdir)/../lib $(all_includes)
LDFLAGS	= $(all_libraries) -version-info 1:0:0

####### Files

lib_LTLIBRARIES = libexcel97.la

libexcel97_la_SOURCES = excelfilter.cc xmltree.cc

noinst_HEADERS = excelfilter.h xmltree.h

METASOURCES = AUTO

service_DATA = ole_excel97_import.desktop
servicedir = $(kde_servicesdir)

data_DATA = x-excel97.desktop
datadir = $(kde_mimedir)/application
