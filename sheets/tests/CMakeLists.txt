include(MacroAddCompileFlags)
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/sheets  ${EIGEN2_INCLUDE_DIR} ${KOMAIN_INCLUDES} ${KDE4_INCLUDES} )

########### Core Functionality ###############

set(TestCell_SRCS TestCell.cpp)
kde4_add_unit_test(TestCell TESTNAME kspread-Cell ${TestCell_SRCS})
target_link_libraries(TestCell calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestCellStorage_SRCS TestCellStorage.cpp)
kde4_add_unit_test(TestCellStorage TESTNAME kspread-CellStorage  ${TestCellStorage_SRCS})
target_link_libraries(TestCellStorage calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestDependencies_SRCS TestDependencies.cpp)
kde4_add_unit_test(TestDependencies TESTNAME kspread-Dependencies ${TestDependencies_SRCS})
target_link_libraries(TestDependencies calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestFormula_SRCS TestFormula.cpp)
kde4_add_unit_test(TestFormula TESTNAME kspread-Formula  ${TestFormula_SRCS})
target_link_libraries(TestFormula calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestUtil_SRCS TestUtil.cpp)
kde4_add_unit_test(TestUtil TESTNAME kspread-Util  ${TestUtil_SRCS})
target_link_libraries(TestUtil calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestOpenFormula_SRCS TestOpenFormula.cpp)
kde4_add_unit_test(TestOpenFormula TESTNAME kspread-OpenFormula  ${TestOpenFormula_SRCS})
target_link_libraries(TestOpenFormula calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestPointStorage_SRCS TestPointStorage.cpp)
kde4_add_unit_test(TestPointStorage TESTNAME kspread-PointStorage  ${TestPointStorage_SRCS})
target_link_libraries(TestPointStorage calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestRegion_SRCS TestRegion.cpp)
kde4_add_unit_test(TestRegion TESTNAME kspread-Region  ${TestRegion_SRCS})
target_link_libraries(TestRegion calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestRTree_SRCS TestRTree.cpp)
kde4_add_unit_test(TestRTree TESTNAME kpsread-RTree  ${TestRTree_SRCS})
target_link_libraries(TestRTree ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestSort_SRCS TestSort.cpp)
kde4_add_unit_test(TestSort TESTNAME kspread-Sort ${TestSort_SRCS})
target_link_libraries(TestSort calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestSelection_SRCS TestSelection.cpp)
kde4_add_unit_test(TestSelection TESTNAME kspread-Selection ${TestSelection_SRCS})
target_link_libraries(TestSelection calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestValue_SRCS TestValue.cpp)
kde4_add_unit_test(TestValue TESTNAME kspread-Value  ${TestValue_SRCS})
target_link_libraries(TestValue calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestStyleStorage_SRCS TestStyleStorage.cpp)
kde4_add_unit_test(TestStyleStorage TESTNAME kspread-StyleStorage ${TestStyleStorage_SRCS})
target_link_libraries(TestStyleStorage calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})


########### Function tests ###############

set(TestBitopsFunctions_SRCS TestBitopsFunctions.cpp)
kde4_add_unit_test(TestBitopsFunctions TESTNAME kspread-BitopsFunctions  ${TestBitopsFunctions_SRCS})
target_link_libraries(TestBitopsFunctions calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestDatabaseFunctions_SRCS TestDatabaseFunctions.cpp)
kde4_add_unit_test(TestDatabaseFunctions TESTNAME kspread-DatabaseFunctions  ${TestDatabaseFunctions_SRCS})
target_link_libraries(TestDatabaseFunctions calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### target TestDatetimeFunctions ###############

set(TestDatetimeFunctions_SRCS TestDatetimeFunctions.cpp)
kde4_add_unit_test(TestDatetimeFunctions TESTNAME kspread-DatetimeFunctions  ${TestDatetimeFunctions_SRCS})
target_link_libraries(TestDatetimeFunctions calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestEngineeringFunctions_SRCS TestEngineeringFunctions.cpp)
kde4_add_unit_test(TestEngineeringFunctions TESTNAME kspread-EngineeringFunctions  ${TestEngineeringFunctions_SRCS})
target_link_libraries(TestEngineeringFunctions calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestFinancialFunctions_SRCS TestFinancialFunctions.cpp)
kde4_add_unit_test(TestFinancialFunctions TESTNAME kspread-FinancialFunctions  ${TestFinancialFunctions_SRCS})
target_link_libraries(TestFinancialFunctions calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestInformationFunctions_SRCS TestInformationFunctions.cpp)
kde4_add_unit_test(TestInformationFunctions TESTNAME kspread-InformationFunctions  ${TestInformationFunctions_SRCS})
target_link_libraries(TestInformationFunctions calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestLogicFunctions_SRCS TestLogicFunctions.cpp)
kde4_add_unit_test(TestLogicFunctions TESTNAME kspread-LogicFunctions  ${TestLogicFunctions_SRCS})
target_link_libraries(TestLogicFunctions calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestMathFunctions_SRCS TestMathFunctions.cpp)
kde4_add_unit_test(TestMathFunctions TESTNAME kspread-MathFunctions  ${TestMathFunctions_SRCS})
target_link_libraries(TestMathFunctions calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestStatisticalFunctions_SRCS TestStatisticalFunctions.cpp)
kde4_add_unit_test(TestStatisticalFunctions TESTNAME kspread-StatisticalFunctions  ${TestStatisticalFunctions_SRCS})
target_link_libraries(TestStatisticalFunctions calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestTextFunctions_SRCS TestTextFunctions.cpp)
kde4_add_unit_test(TestTextFunctions TESTNAME kspread-TextFunctions  ${TestTextFunctions_SRCS})
target_link_libraries(TestTextFunctions calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestTrigFunctions_SRCS TestTrigFunctions.cpp)
kde4_add_unit_test(TestTrigFunctions TESTNAME kspread-TrigFunctions  ${TestTrigFunctions_SRCS})
target_link_libraries(TestTrigFunctions calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestValueFormatter_SRCS TestValueFormatter.cpp)
kde4_add_unit_test(TestValueFormatter TESTNAME kspread-ValueFormatter  ${TestValueFormatter_SRCS})
macro_add_compile_flags(TestValueFormatter "-DCALLIGRA_SHEETS_UNIT_TEST")
target_link_libraries(TestValueFormatter calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestSheet_SRCS TestSheet.cpp)
kde4_add_unit_test(TestSheet TESTNAME kspread-Sheet  ${TestSheet_SRCS})
macro_add_compile_flags(TestSheet "-DCALLIGRA_SHEETS_UNIT_TEST")
target_link_libraries(TestSheet calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestGraphicsView_SRCS TestGraphicsView.cpp)
kde4_add_executable(TestGraphicsView TEST ${TestGraphicsView_SRCS})
target_link_libraries(TestGraphicsView calligrasheetscommon)

########### next target ###############

set(TestRowRepeatStorage_SRCS TestRowRepeatStorage.cpp)
kde4_add_unit_test(TestRowRepeatStorage TESTNAME kspread-RowRepeatStorage ${TestRowRepeatStorage_SRCS})
target_link_libraries(TestRowRepeatStorage calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestRowFormatStorage_SRCS TestRowFormatStorage.cpp)
kde4_add_unit_test(TestRowFormatStorage TESTNAME kspread-RowFormatStorage ${TestRowFormatStorage_SRCS})
target_link_libraries(TestRowFormatStorage calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestDatabaseFilter_SRCS TestDatabaseFilter.cpp)
kde4_add_unit_test(TestDatabaseFilter TESTNAME kspread-Database-Filter ${TestDatabaseFilter_SRCS})
target_link_libraries(TestDatabaseFilter calligrasheetsodf ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestPasteCommand_SRCS TestPasteCommand.cpp)
kde4_add_unit_test(TestPasteCommand TESTNAME kspread-PasteCommand ${TestPasteCommand_SRCS})
target_link_libraries(TestPasteCommand calligrasheetscommon ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### Benchmarks ###############

# set(BenchmarkCluster_SRCS BenchmarkCluster.cpp ../Cluster.cpp) # explicit Cluster.cpp for no extra symbol visibility
# kde4_add_executable(BenchmarkCluster TEST ${BenchmarkCluster_SRCS})
# target_link_libraries(BenchmarkCluster calligrasheetscommon ${QT_QTTEST_LIBRARY})

########### next target ###############

set(BenchmarkPointStorage_SRCS BenchmarkPointStorage.cpp)
kde4_add_executable(BenchmarkPointStorage TEST ${BenchmarkPointStorage_SRCS})
target_link_libraries(BenchmarkPointStorage ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(BenchmarkRTree_SRCS BenchmarkRTree.cpp)
kde4_add_executable(BenchmarkRTree TEST ${BenchmarkRTree_SRCS})
target_link_libraries(BenchmarkRTree ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
