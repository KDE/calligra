
lib_LTLIBRARIES = libkoml.la
LDFLAGS = -version-info 1:0:0
INCLUDES = $(MICO_INCLUDES)

####### Files

libkoml_la_SOURCES = komlParser.cc komlFeed.cc komlFileFeed.cc \
	komlWriter.cc komlStreamFeed.cc torben.cc

include_HEADERS = komlParser.h komlFeed.h komlFileFeed.h \
	komlWriter.h torben.h komlStreamFeed.h

SUFFIXES = .h .moc

.h.moc:
	$(MOC) $< -o $@      

idl:

