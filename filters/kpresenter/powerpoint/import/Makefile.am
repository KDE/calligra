INCLUDES = -I$(srcdir) -I$(srcdir)/../libppt $(KOFFICE_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES = libpowerpointimport.la

libpowerpointimport_la_SOURCES = powerpointimport.cc
libpowerpointimport_la_LIBADD  = $(KOFFICE_LIBS) ../libppt/libppt.la
libpowerpointimport_la_LDFLAGS = -module $(KDE_PLUGIN)
noinst_HEADERS = powerpointimport.h

METASOURCES = AUTO

service_DATA = kpresenter_powerpoint_import.desktop
servicedir = $(kde_servicesdir)

