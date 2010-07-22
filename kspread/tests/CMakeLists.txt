include(MacroAddCompileFlags)
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/kspread  ${EIGEN2_INCLUDE_DIR} ${KOMAIN_INCLUDES} ${KDE4_INCLUDES} )

########### Core Functionality ###############

set(TestDependencies_SRCS TestDependencies.cpp)
kde4_add_unit_test(TestDependencies TESTNAME kspread-Dependencies ${TestDependencies_SRCS})
target_link_libraries(TestDependencies kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestFormula_SRCS TestFormula.cpp)
kde4_add_unit_test(TestFormula TESTNAME kspread-Formula  ${TestFormula_SRCS})
target_link_libraries(TestFormula kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestOpenFormula_SRCS TestOpenFormula.cpp)
kde4_add_unit_test(TestOpenFormula TESTNAME kspread-OpenFormula  ${TestOpenFormula_SRCS})
target_link_libraries(TestOpenFormula kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestPointStorage_SRCS TestPointStorage.cpp)
kde4_add_unit_test(TestPointStorage TESTNAME kspread-PointStorage  ${TestPointStorage_SRCS})
target_link_libraries(TestPointStorage kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestRegion_SRCS TestRegion.cpp)
kde4_add_unit_test(TestRegion TESTNAME kspread-Region  ${TestRegion_SRCS})
target_link_libraries(TestRegion kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestRTree_SRCS TestRTree.cpp)
kde4_add_unit_test(TestRTree TESTNAME kpsread-RTree  ${TestRTree_SRCS})
target_link_libraries(TestRTree ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestSelection_SRCS TestSelection.cpp)
kde4_add_unit_test(TestSelection TESTNAME kspread-Selection ${TestSelection_SRCS})
target_link_libraries(TestSelection kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestValue_SRCS TestValue.cpp)
kde4_add_unit_test(TestValue TESTNAME kspread-Value  ${TestValue_SRCS})
target_link_libraries(TestValue kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})



########### Function tests ###############

set(TestBitopsFunctions_SRCS TestBitopsFunctions.cpp)
kde4_add_unit_test(TestBitopsFunctions TESTNAME kspread-BitopsFunctions  ${TestBitopsFunctions_SRCS})
target_link_libraries(TestBitopsFunctions kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestDatabaseFunctions_SRCS TestDatabaseFunctions.cpp)
kde4_add_unit_test(TestDatabaseFunctions TESTNAME kspread-DatabaseFunctions  ${TestDatabaseFunctions_SRCS})
target_link_libraries(TestDatabaseFunctions kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### target TestDatetimeFunctions ###############

set(TestDatetimeFunctions_SRCS TestDatetimeFunctions.cpp)
kde4_add_unit_test(TestDatetimeFunctions TESTNAME kspread-DatetimeFunctions  ${TestDatetimeFunctions_SRCS})
target_link_libraries(TestDatetimeFunctions kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestEngineeringFunctions_SRCS TestEngineeringFunctions.cpp)
kde4_add_unit_test(TestEngineeringFunctions TESTNAME kspread-EngineeringFunctions  ${TestEngineeringFunctions_SRCS})
target_link_libraries(TestEngineeringFunctions kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestFinancialFunctions_SRCS TestFinancialFunctions.cpp)
kde4_add_unit_test(TestFinancialFunctions TESTNAME kspread-FinancialFunctions  ${TestFinancialFunctions_SRCS})
target_link_libraries(TestFinancialFunctions kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestInformationFunctions_SRCS TestInformationFunctions.cpp)
kde4_add_unit_test(TestInformationFunctions TESTNAME kspread-InformationFunctions  ${TestInformationFunctions_SRCS})
target_link_libraries(TestInformationFunctions kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestLogicFunctions_SRCS TestLogicFunctions.cpp)
kde4_add_unit_test(TestLogicFunctions TESTNAME kspread-LogicFunctions  ${TestLogicFunctions_SRCS})
target_link_libraries(TestLogicFunctions kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestMathFunctions_SRCS TestMathFunctions.cpp)
kde4_add_unit_test(TestMathFunctions TESTNAME kspread-MathFunctions  ${TestMathFunctions_SRCS})
target_link_libraries(TestMathFunctions kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestStatisticalFunctions_SRCS TestStatisticalFunctions.cpp)
kde4_add_unit_test(TestStatisticalFunctions TESTNAME kspread-StatisticalFunctions  ${TestStatisticalFunctions_SRCS})
target_link_libraries(TestStatisticalFunctions kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestTextFunctions_SRCS TestTextFunctions.cpp)
kde4_add_unit_test(TestTextFunctions TESTNAME kspread-TextFunctions  ${TestTextFunctions_SRCS})
target_link_libraries(TestTextFunctions kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestTrigFunctions_SRCS TestTrigFunctions.cpp)
kde4_add_unit_test(TestTrigFunctions TESTNAME kspread-TrigFunctions  ${TestTrigFunctions_SRCS})
target_link_libraries(TestTrigFunctions kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestValueFormatter_SRCS TestValueFormatter.cpp)
kde4_add_unit_test(TestValueFormatter TESTNAME kspread-ValueFormatter  ${TestValueFormatter_SRCS})
macro_add_compile_flags(TestValueFormatter "-DKSPREAD_UNIT_TEST")
target_link_libraries(TestValueFormatter kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestSheet_SRCS TestSheet.cpp)
kde4_add_unit_test(TestSheet TESTNAME kspread-Sheet  ${TestSheet_SRCS})
macro_add_compile_flags(TestSheet "-DKSPREAD_UNIT_TEST")
target_link_libraries(TestSheet kspreadcommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestGraphicsView_SRCS TestGraphicsView.cpp)
kde4_add_executable(TestGraphicsView TEST ${TestGraphicsView_SRCS})
target_link_libraries(TestGraphicsView kspreadcommon)

########### Benchmarks ###############

# set(BenchmarkCluster_SRCS BenchmarkCluster.cpp ../Cluster.cpp) # explicit Cluster.cpp for no extra symbol visibility
# kde4_add_executable(BenchmarkCluster TEST ${BenchmarkCluster_SRCS})
# target_link_libraries(BenchmarkCluster kspreadcommon ${QT_QTTEST_LIBRARY})

########### next target ###############

set(BenchmarkPointStorage_SRCS BenchmarkPointStorage.cpp)
kde4_add_executable(BenchmarkPointStorage TEST ${BenchmarkPointStorage_SRCS})
target_link_libraries(BenchmarkPointStorage ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(BenchmarkRTree_SRCS BenchmarkRTree.cpp)
kde4_add_executable(BenchmarkRTree TEST ${BenchmarkRTree_SRCS})
target_link_libraries(BenchmarkRTree ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
