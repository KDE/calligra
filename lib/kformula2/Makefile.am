
INCLUDES= $(all_includes)

SUBDIRS = 

####### 
# you can add here more. This one gets installed 
lib_LTLIBRARIES = libkformula2.la
KFORMULA2VERSION=	0.0.1
KFORMULA2PATCH=	0

libkformula2_la_SOURCES	= basicelement.cc contextstyle.cc formulacursor.cc \
	formulaelement.cc indexelement.cc kformulacontainer.cc kformulawidget.cc \
	sequenceelement.cc textelement.cc bracketelement.cc \
	matrixelement.cc fractionelement.cc rootelement.cc symbolelement.cc \
	artwork.cc kformulacommand.cc kformulamimesource.cc \
	MatrixDialog.cc sequenceparser.cc elementtype.cc kformuladocument.cc \
	symboltable.cc textsymbolelement.cc

include_HEADERS =
#include_HEADERS =  artwork.h formulaelement.h symbolelement.h \
#	basicelement.h fractionelement.h kformulawidget.h \
#	bracketelement.h indexelement.h matrixelement.h \
#	contextstyle.h kformulacommand.h numberelement.h \
#	elementindex.h kformulacontainer.h operatorelement.h textelement.h \
#	formulacursor.h rootelement.h \
#	formuladefs.h kformulamimesource.h sequenceelement.h

libkformula2_la_LDFLAGS = -version-info 3:0 -no-undefined
libkformula2_la_LIBADD  = $(LIB_KDEUI) $(all_libraries) $(KDE_RPATH)

libkformula2_la_METASOURCES = AUTO

check_PROGRAMS = kformulatest
kformulatest_SOURCES = main.cc
kformulatest_LDADD = libkformula2.la
