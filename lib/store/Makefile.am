
lib_LTLIBRARIES = libkstore.la
LDFLAGS = -version-info 1:0:0 $(all_libraries) $(MICO_LDFLAGS)
INCLUDES = $(MICO_INCLUDES) $(all_includes)
IDL_FLAGS = --relative-paths --query-server-for-narrow

####### Files

libkstore_la_LIBADD = $(LIB_KDECORE) $(LIBMICO)
libkstore_la_SOURCES = koStoreIf.idl koStore.cc
include_HEADERS = koStore.h

idl_DATA = koStoreIf.idl

