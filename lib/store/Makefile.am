
KDE_CXXFLAGS = $(USE_RTTI)
lib_LTLIBRARIES = libkstore.la
INCLUDES = $(all_includes)

####### Files

libkstore_la_LIBADD = $(LIB_KSYCOCA)
libkstore_la_SOURCES = koStore.cc
libkstore_la_LDFLAGS = -version-info 1:0:0 $(KDE_LDFLAGS) -no-undefined
include_HEADERS = koStore.h koStoreDevice.h
