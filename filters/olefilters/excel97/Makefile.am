####### General stuff

INCLUDES= -I$(srcdir) -I$(srcdir)/../lib $(all_includes)
LDFLAGS	= $(all_libraries)

####### Files

noinst_LTLIBRARIES = libexcel97.la

libexcel97_la_SOURCES = helper.cc excelfilter.cc handler.cc worker.cc

noinst_HEADERS = excelfilter.h handler.h worker.h helper.h definitions.h

METASOURCES = AUTO

service_DATA = ole_excel97_import.desktop
servicedir = $(kde_servicesdir)
