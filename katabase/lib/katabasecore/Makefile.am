####### General stuff

INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES) $(PGSQLINC)
LDFLAGS = $(all_libraries) $(MICO_LDFLAGS) $(PGSQLLIB) $(KATABASE_LIBS) -version-info 1:0:0
libkatabasecore_la_LIBADD = ../../../lib/kofficeui/libkofficeui.la

####### Files

lib_LTLIBRARIES = libkatabasecore.la

libkatabasecore_la_SOURCES = katabase.idl kbDatabase.cc kbRecordSet.cc kbTable.cc

noinst_HEADERS = kbDatabase.h kbRecordSet.h kbTable.h

idl_DATA = katabase.idl

IDL_FLAGS = --relative-paths --any -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(srcdir) -I$(idldir)

