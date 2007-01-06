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

set(TestFinancialFunctions_SRCS TestFinancialFunctions.cpp)

kde4_automoc(${TestFinancialFunctions_SRCS})

kde4_add_executable(TestFinancialFunctions RUN_UNINSTALLED ${TestFinancialFunctions_SRCS})

target_link_libraries(TestFinancialFunctions kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(FinancialFunctions TestFinancialFunctions)

########### next target ###############

set(TestTrigFunctions_SRCS TestTrigFunctions.cpp)

kde4_automoc(${TestTrigFunctions_SRCS})

kde4_add_executable(TestTrigFunctions RUN_UNINSTALLED ${TestTrigFunctions_SRCS})

target_link_libraries(TestTrigFunctions kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(TrigFunctions TestTrigFunctions)








endif(KDE4_BUILD_TESTS)
