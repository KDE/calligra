####### General stuff

INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES) $(PGSQLINC)
LDFLAGS = $(all_libraries) $(MICO_LDFLAGS) $(PGSQLLIB) $(KATABASE_LIBS) -version-info 1:0:0
# libkatabasecore_la_LIBADD = ../../../lib/kofficeui/libkofficeui.la

####### Files

lib_LTLIBRARIES = libkdbcore.la

libkdbcore_la_SOURCES = kdb.idl kdbDBProperties.cc

include_HEADERS = kdb.h kdbDBProperties.h

idl_DATA = kdb.idl

IDL_FLAGS = --query-server-for-narrow --relative-paths --any -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(srcdir) -I$(idldir)

