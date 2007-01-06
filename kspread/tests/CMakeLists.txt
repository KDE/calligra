if(KDE4_BUILD_TESTS)

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KOFFICEUI_INCLUDES} )

########### next target ###############

set(TestFormula_SRCS TestFormula.cpp)

kde4_automoc(${TestFormula_SRCS})

kde4_add_executable(TestFormula RUN_UNINSTALLED ${TestFormula_SRCS})

target_link_libraries(TestFormula kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(Formula TestFormula)

########### next target ###############

set(TestOpenFormula_SRCS TestOpenFormula.cpp)

kde4_automoc(${TestOpenFormula_SRCS})

kde4_add_executable(TestOpenFormula RUN_UNINSTALLED ${TestOpenFormula_SRCS})

target_link_libraries(TestOpenFormula kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(OpenFormula TestOpenFormula)

########### next target ###############

set(BenchmarkRTree_SRCS BenchmarkRTree.cpp)

kde4_automoc(${BenchmarkRTree_SRCS})

kde4_add_executable(BenchmarkRTree RUN_UNINSTALLED ${BenchmarkRTree_SRCS})

target_link_libraries(BenchmarkRTree kdecore)

# add_test(R-Tree BenchmarkRTreetest)

########### next target ###############

set(TestRTree_SRCS TestRTree.cpp)

kde4_automoc(${TestRTree_SRCS})

kde4_add_executable(TestRTree RUN_UNINSTALLED ${TestRTree_SRCS})

target_link_libraries(TestRTree kdecore ${QT_QTTEST_LIBRARY})

add_test(R-Tree TestRTree)

########### next target ###############

set(TestValue_SRCS TestValue.cpp)

kde4_automoc(${TestValue_SRCS})

kde4_add_executable(TestValue RUN_UNINSTALLED ${TestValue_SRCS})

target_link_libraries(TestValue kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(Value TestValue)

########### next target ###############

set(TestMathFunctions_SRCS TestMathFunctions.cpp)

kde4_automoc(${TestMathFunctions_SRCS})

kde4_add_executable(TestMathFunctions RUN_UNINSTALLED ${TestMathFunctions_SRCS})

target_link_libraries(TestMathFunctions kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(MathFunctions TestMathFunctions)

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

########### next target ###############

set(BenchmarkCluster_SRCS BenchmarkCluster.cpp ../Cluster.cpp) # explicit Cluster.cpp for no extra symbol visibility

kde4_automoc(${BenchmarkCluster_SRCS})

kde4_add_executable(BenchmarkCluster RUN_UNINSTALLED ${BenchmarkCluster_SRCS})

target_link_libraries(BenchmarkCluster kspreadcommon ${QT_QTTEST_LIBRARY})

#add_test(Cluster BenchmarkCluster)


endif(KDE4_BUILD_TESTS)
