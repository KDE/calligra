
INCLUDES= $(all_includes) $(KOFFICE_INCLUDES)

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
	symboltable.cc kformulamathmlread.cc \
	kformulainputfilter.cc kformulaview.cc kformulacompatibility.cc

include_HEADERS =

libkformula2_la_LDFLAGS = -version-info 3:0 -no-undefined
libkformula2_la_LIBADD  = $(LIB_KDEUI) $(all_libraries) $(KDE_RPATH)

libkformula2_la_METASOURCES = AUTO

check_PROGRAMS = kformulatest unittest

kformulatest_SOURCES = main.cc
kformulatest_LDADD = libkformula2.la

# needs the CppUnit from http://www.xProgramming.com/software.htm
unittest_SOURCES = testcommands.cc testformulacursor.cc testindexelement.cc testmain.cc
unittest_LDADD = libkformula2.la libCppUnit.so libCppUnitRunner.so
