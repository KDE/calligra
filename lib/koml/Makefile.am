
lib_LTLIBRARIES = libkoml.la
LDFLAGS = -version-info 1:0:0 $(all_libraries)
INCLUDES = $(MICO_INCLUDES) -I$(idldir) $(all_includes)

####### Files

libkoml_la_SOURCES = komlParser.cc komlFeed.cc komlFileFeed.cc \
	komlWriter.cc komlStreamFeed.cc torben.cc komlMime.cc

include_HEADERS = komlParser.h komlFeed.h komlFileFeed.h \
	komlWriter.h torben.h komlStreamFeed.h komlMime.h

libkoml_la_LIBADD = $(LIB_KDECORE)
