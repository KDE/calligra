KDE_CXXFLAGS = $(USE_RTTI)

INCLUDES = \
	$(KOFFICE_INCLUDES) \
	$(KOTEXT_INCLUDES) \
	-I$(srcdir)/.. \
	$(all_includes)

check_PROGRAMS = formulatest
TESTS = formulatest
formulatest_SOURCES = test_formula.cc
formulatest_LDADD = ../libkspreadcommon.la


noinst_LTLIBRARIES = \
	libtests.la

libtests_la_SOURCES = \
	tester.cc \
	testrunner.cc \
	value_tester.cc \
	formula_tester.cc \
	inspector.cc

noinst_HEADERS = \
	tester.h \
	testrunner.h \
	value_tester.h \
	formula_tester.h \
	inspector.h

METASOURCES = AUTO

