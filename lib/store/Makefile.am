
KDE_CXXFLAGS = $(USE_RTTI) 
lib_LTLIBRARIES = libkstore.la
INCLUDES = $(all_includes)

####### Files

libkstore_la_LIBADD = $(LIB_KIO)
libkstore_la_SOURCES = koTarStore.cc koStoreStream.cc koBinaryStore.cc
libkstore_la_LDFLAGS = -version-info 1:0:0 $(KDE_LDFLAGS) -no-undefined
include_HEADERS = koStore.h koTarStore.h koStoreStream.h koBinaryStore.h
