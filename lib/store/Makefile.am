
KDE_CXXFLAGS = $(USE_RTTI) 
lib_LTLIBRARIES = libkstore.la
LDFLAGS = -version-info 1:0:0 $(all_libraries)
INCLUDES = $(all_includes)
IDL_FLAGS = --relative-paths

####### Files

libkstore_la_LIBADD = $(LIB_KDECORE)
libkstore_la_SOURCES = koTarStore.cc koStoreStream.cc koBinaryStore.cc
include_HEADERS = koStore.h koTarStore.h koStoreStream.h koBinaryStore.h


