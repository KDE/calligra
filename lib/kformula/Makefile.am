
INCLUDES= $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes) 

SUBDIRS = pics dtd

####### 
# you can add here more. This one gets installed 
lib_LTLIBRARIES = libkformula.la

libkformula_la_SOURCES	= basicelement.cc contextstyle.cc formulacursor.cc \
	formulaelement.cc indexelement.cc kformulacontainer.cc \
	sequenceelement.cc textelement.cc bracketelement.cc \
	matrixelement.cc fractionelement.cc rootelement.cc symbolelement.cc \
	kformulacommand.cc kformulamimesource.cc \
	MatrixDialog.cc sequenceparser.cc elementtype.cc kformuladocument.cc \
	symboltable.cc kformulamathmlread.cc \
	kformulainputfilter.cc kformulaview.cc \
	spaceelement.cc kformulaconfigpage.cc \
	symbolaction.cc cmstyle.cc \
	fontstyle.cc symbolfontstyle.cc esstixfontstyle.cc creationstrategy.cc

#include_HEADERS = kformulacontainer.h kformuladocument.h kformulaview.h \
#	kformuladefs.h kformulaconfigpage.h

libkformula_la_LDFLAGS = -version-info 3:0 -no-undefined
libkformula_la_LIBADD  = $(LIB_KDEUI) $(LIB_KOTEXT) $(all_libraries) $(KDE_RPATH)

libkformula_la_METASOURCES = AUTO

check_PROGRAMS = kformulatest

kformulatest_SOURCES = main.cc kformulawidget.cc
kformulatest_LDADD = libkformula.la

#symbolnames.cc: 
#	awk -F, '$$1 !~ "#" {if (split($$3,a," ")>0) print "i18n(\"" a[1] "\");"}' config/unicode.tbl > symbolnames.cc
