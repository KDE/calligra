####### General stuff

INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES) $(PGSQLINC) -I../kdbcore
LDFLAGS = $(all_libraries) $(MICO_LDFLAGS) $(PGSQLLIB) $(KATABASE_LIBS) -version-info 1:0:0
# libkatabasecore_la_LIBADD = ../../../lib/kofficeui/libkofficeui.la

####### Files

lib_LTLIBRARIES = libkdbpgsql.la

libkdbpgsql_la_SOURCES = pgsqlDataSource.cc

include_HEADERS = pgsqlDataSource.h

idl_DATA = 

IDL_FLAGS = --relative-paths --any -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(srcdir) -I$(idldir)

