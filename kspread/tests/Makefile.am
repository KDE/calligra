KDE_CXXFLAGS = $(USE_RTTI)

INCLUDES = \
	$(KOFFICE_INCLUDES) \
	$(KOSCRIPT_INCLUDES) \
	$(KOTEXT_INCLUDES) \
	-I$(srcdir)/.. \
	$(all_includes)

noinst_LTLIBRARIES = \
	libtests.la

libtests_la_SOURCES = \
	tester.cc \
	testrunner.cc \
	value_tester.cc \
	formula_tester.cc

noinst_HEADERS = \
	tester.h \
	testrunner.h \
	value_tester.h \
	formula_tester.h
        
METASOURCES = AUTO

