
KDE_CXXFLAGS = $(USE_RTTI)
lib_LTLIBRARIES = libkoml.la
INCLUDES = $(all_includes)

####### Files

libkoml_la_SOURCES = komlParser.cc komlWriter.cc komlMime.cc

include_HEADERS = komlParser.h komlWriter.h komlMime.h

libkoml_la_LIBADD = $(LIB_KDECORE)
libkoml_la_LDFLAGS = -version-info 1:0:0 $(KDE_LDFLAGS) -no-undefined
