

if(KDE4_BUILD_TESTS)

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KOFFICEUI_INCLUDES} )

set(libtests_SRCS tester.cc testrunner.cc value_tester.cc formula_tester.cc inspector.cc)

########### next target ###############

set(formulatest_SRCS test_formula.cc ${libtests_SRCS})

kde4_automoc(${formulatest_SRCS})

kde4_add_executable(formulatest ${formulatest_SRCS})

target_link_libraries(formulatest  kspreadcommon )

add_test(formula ${EXECUTABLE_OUTPUT_PATH}/formulatest)


########### next target ###############


########### install files ###############


endif(KDE4_BUILD_TESTS)





#original Makefile.am contents follow:

#KDE_CXXFLAGS = $(USE_RTTI)
#
#INCLUDES = \
#	$(KOFFICE_INCLUDES) \
#	$(KOTEXT_INCLUDES) \
#	-I$(srcdir)/.. \
#	$(all_includes)
#
#check_PROGRAMS = formulatest
#TESTS = formulatest
#formulatest_SOURCES = test_formula.cc
#formulatest_LDADD = ../libkspreadcommon.la
#
#
#noinst_LTLIBRARIES = \
#	libtests.la
#
#libtests_la_SOURCES = \
#	tester.cc \
#	testrunner.cc \
#	value_tester.cc \
#	formula_tester.cc \
#	inspector.cc
#
#noinst_HEADERS = \
#	tester.h \
#	testrunner.h \
#	value_tester.h \
#	formula_tester.h \
#	inspector.h
#
#METASOURCES = AUTO
#
