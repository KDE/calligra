
KDE_CXXFLAGS = $(USE_RTTI)
lib_LTLIBRARIES = libkoml.la
INCLUDES = $(all_includes)

####### Files

libkoml_la_SOURCES = komlWrapper.cc komlParser.cc komlFeed.cc komlFileFeed.cc \
	komlWriter.cc komlStreamFeed.cc torben.cc komlMime.cc

include_HEADERS = komlWrapper.h komlParser.h komlFeed.h komlFileFeed.h \
	komlWriter.h torben.h komlStreamFeed.h komlMime.h

libkoml_la_LIBADD = $(LIB_KDECORE)
libkoml_la_LDFLAGS = -version-info 1:0:0 $(KDE_LDFLAGS) -no-undefined
