
INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes)
KDE_CXXFLAGS = $(USE_RTTI)

####### Files

bin_PROGRAMS = kformula
lib_LTLIBRARIES = kformulamain.la libkformulapart.la

libkformulapart_la_SOURCES = kformula_doc.cc \
	kformula_view.cc kformula_factory.cc BasicElement.cc \
	RootElement.cc TextElement.cc BracketElement.cc FractionElement.cc\
	MatrixElement.cc PrefixedElement.cc formula_container.cc matrixwidget.cc
libkformulapart_la_LDFLAGS = -module -avoid-version -no-undefined
libkformulapart_la_LIBADD = ../lib/kofficeui/libkofficeui.la
libkformulapart_la_METASOURCES = AUTO

kformulamain_la_SOURCES = main.cc
kformulamain_la_LDFLAGS = $(all_libraries) -module -avoid-version
kformulamain_la_LIBADD = libkformulapart.la

kformula_SOURCES = dummy.cc
kformula_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kformula_LDADD = kformulamain.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS =  kformula_doc.h \
		  kformula_view.h kformula_factory.h  formuladef.h \
		  BasicElement.h RootElement.h TextElement.h\
		  BracketElement.h FractionElement.h MatrixElement.h PrefixedElement.h \
		  matrixwidget.h formula_container.h

kdelnk_DATA = kformula.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-kformula.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = kformula.rc
rcdir = $(kde_datadir)/kformula

SUBDIRS =  pics

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc -o $(podir)/kformula.pot
