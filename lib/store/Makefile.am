
lib_LTLIBRARIES = libkstore.la
LDFLAGS = -version-info 1:0:0 $(all_libraries) $(MICO_LDFLAGS)
INCLUDES = $(MICO_INCLUDES) $(all_includes)
IDL_FLAGS = --relative-paths

####### Files

libkstore_la_LIBADD = $(LIB_KDECORE) $(LIBMICO)
libkstore_la_SOURCES = koStoreIf.idl koTarStore.cc koStoreStream.cc koBinaryStore.cc
include_HEADERS = koTarStore.h koStoreStream.h koBinaryStore.h

idl_DATA = koStoreIf.idl

