
SUBDIRS = . tests

KDE_CXXFLAGS = $(USE_RTTI)
lib_LTLIBRARIES = libkstore.la
INCLUDES = $(KOFFICECORE_INCLUDES) $(all_includes)

####### Files

libkstore_la_LIBADD = $(LIB_KSYCOCA)
libkstore_la_SOURCES = koStore.cc koTarStore.cc koDirectoryStore.cc koZipStore.cc koStoreDrag.cc koStoreBase.cc koxmlwriter.cc
libkstore_la_LDFLAGS = $(all_libraries) -version-info 2:0:0 $(KDE_LDFLAGS) -no-undefined
include_HEADERS = koStore.h koStoreDevice.h koxmlwriter.h
