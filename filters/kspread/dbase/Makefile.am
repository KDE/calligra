####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

####### Files

kde_module_LTLIBRARIES = libdbaseimport.la

libdbaseimport_la_SOURCES = dbaseimport.cc dbase.cpp
libdbaseimport_la_LIBADD = $(KOFFICE_LIBS)
libdbaseimport_la_LDFLAGS = -module $(KDE_PLUGIN)
noinst_HEADERS = dbaseimport.h dbase.h

METASOURCES = AUTO

service_DATA = kspread_dbase_import.desktop
servicedir = $(kde_servicesdir)
