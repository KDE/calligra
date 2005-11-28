####### General stuff

INCLUDES= -I$(srcdir) -I$(srcdir)/../lib $(KOFFICE_INCLUDES) $(all_includes)

####### Files

noinst_LTLIBRARIES = libhancomword.la

libhancomword_la_SOURCES = hancomwordfilter.cc 
libhancomword_la_LDFLAGS = $(all_libraries)

noinst_HEADERS = hancomwordfilter.h

METASOURCES = AUTO

service_DATA = ole_hancomword_import.desktop
servicedir = $(kde_servicesdir)
