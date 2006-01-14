####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

####### Files

kde_module_LTLIBRARIES = libhancomwordimport.la

libhancomwordimport_la_SOURCES = pole.cpp hancomwordimport.cpp
libhancomwordimport_la_LIBADD = $(KOFFICE_LIBS) 
libhancomwordimport_la_LDFLAGS = -module $(KDE_PLUGIN)
noinst_HEADERS = pole.h hancomwordimport.h

METASOURCES = AUTO

service_DATA = kword_hancomword_import.desktop
servicedir = $(kde_servicesdir)
