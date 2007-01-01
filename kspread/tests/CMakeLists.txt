if(KDE4_BUILD_TESTS)

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KOFFICEUI_INCLUDES} )

set(libtests_SRCS tester.cc testrunner.cc value_tester.cc formula_tester.cc inspector.cc)

########### next target ###############

set(formulatest_SRCS test_formula.cc ${libtests_SRCS})

kde4_automoc(${formulatest_SRCS})

kde4_add_executable(formulatest RUN_UNINSTALLED ${formulatest_SRCS})

target_link_libraries(formulatest  kspreadcommon )

add_test(formula formulatest)

########### next target ###############

set(rtreeperformance_SRCS rtreeperformancetest.cpp)

kde4_automoc(${rtreeperformance_SRCS})

kde4_add_executable(rtreeperformancetest RUN_UNINSTALLED ${rtreeperformance_SRCS})

target_link_libraries(rtreeperformancetest kdecore)

# add_test(R-Tree rtreeperformancetest)

########### next target ###############

set(rtreetest_SRCS rtreetest.cpp)

kde4_automoc(${rtreetest_SRCS})

kde4_add_executable(rtreetest RUN_UNINSTALLED ${rtreetest_SRCS})

target_link_libraries(rtreetest kdecore ${QT_QTTEST_LIBRARY})

add_test(R-Tree rtreetest)

########### next target ###############

set(valuetest_SRCS valuetest.cpp)

kde4_automoc(${valuetest_SRCS})

kde4_add_executable(valuetest RUN_UNINSTALLED ${valuetest_SRCS})

target_link_libraries(valuetest kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(Value valuetest)

########### next target ###############

set(FinancialFunctionsTest_SRCS FinancialFunctionsTest.cpp)

kde4_automoc(${FinancialFunctionsTest_SRCS})

kde4_add_executable(FinancialFunctionsTest RUN_UNINSTALLED ${FinancialFunctionsTest_SRCS})

target_link_libraries(FinancialFunctionsTest kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(FinancialFunctions FinancialFunctionsTest)

########### next target ###############

set(MathFunctionsTest_SRCS MathFunctionsTest.cpp)

kde4_automoc(${MathFunctionsTest_SRCS})

kde4_add_executable(MathFunctionsTest RUN_UNINSTALLED ${MathFunctionsTest_SRCS})

target_link_libraries(MathFunctionsTest kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(MathFunctions MathFunctionsTest)

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
