
lib_LTLIBRARIES = libkoml.la

####### Library version

SOREV 	=	1.0.0
SOMAJOR = 	1
SOMINOR = 	0
SOPATCH =	0	

####### Files

libkoml_la_SOURCES = komlParser.cc komlFeed.cc komlFileFeed.cc \
	komlWriter.cc komlStreamFeed.cc torben.cc

include_HEADERS = komlParser.h komlFeed.h komlFileFeed.h \
	komlWriter.h torben.h komlStreamFeed.h

SUFFIXES = .h .moc

.h.moc:
	$(MOC) $< -o $@      

idl:

