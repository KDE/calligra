include(MacroAddCompileFlags)
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/sheets  ${EIGEN3_INCLUDE_DIR} ${KOMAIN_INCLUDES}  )

########### Core Functionality ###############

set(TestCell_SRCS TestCell.cpp)
add_executable(TestCellNAME sheets-Cell ${TestCell_SRCS})
add_test(TestCell TestCell)
ecm_mark_as_test(TestCell)
target_link_libraries(TestCell calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestCellStorage_SRCS TestCellStorage.cpp)
add_executable(TestCellStorageNAME sheets-CellStorage  ${TestCellStorage_SRCS})
add_test(TestCellStorage TestCellStorage)
ecm_mark_as_test(TestCellStorage)
target_link_libraries(TestCellStorage calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestDependencies_SRCS TestDependencies.cpp)
add_executable(TestDependenciesNAME sheets-Dependencies ${TestDependencies_SRCS})
add_test(TestDependencies TestDependencies)
ecm_mark_as_test(TestDependencies)
target_link_libraries(TestDependencies calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestFormula_SRCS TestFormula.cpp)
add_executable(TestFormulaNAME sheets-Formula  ${TestFormula_SRCS})
add_test(TestFormula TestFormula)
ecm_mark_as_test(TestFormula)
target_link_libraries(TestFormula calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestUtil_SRCS TestUtil.cpp)
add_executable(TestUtilNAME sheets-Util  ${TestUtil_SRCS})
add_test(TestUtil TestUtil)
ecm_mark_as_test(TestUtil)
target_link_libraries(TestUtil calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestOpenFormula_SRCS TestOpenFormula.cpp)
add_executable(TestOpenFormulaNAME sheets-OpenFormula  ${TestOpenFormula_SRCS})
add_test(TestOpenFormula TestOpenFormula)
ecm_mark_as_test(TestOpenFormula)
target_link_libraries(TestOpenFormula calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestPointStorage_SRCS TestPointStorage.cpp)
add_executable(TestPointStorageNAME sheets-PointStorage  ${TestPointStorage_SRCS})
add_test(TestPointStorage TestPointStorage)
ecm_mark_as_test(TestPointStorage)
target_link_libraries(TestPointStorage calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestRegion_SRCS TestRegion.cpp)
add_executable(TestRegionNAME sheets-Region  ${TestRegion_SRCS})
add_test(TestRegion TestRegion)
ecm_mark_as_test(TestRegion)
target_link_libraries(TestRegion calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestRTree_SRCS TestRTree.cpp)
add_executable(TestRTreeNAME sheets-RTree  ${TestRTree_SRCS})
add_test(TestRTree TestRTree)
ecm_mark_as_test(TestRTree)
target_link_libraries(TestRTree  Qt5::Test)

########### next target ###############

set(TestSort_SRCS TestSort.cpp)
add_executable(TestSortNAME sheets-Sort ${TestSort_SRCS})
add_test(TestSort TestSort)
ecm_mark_as_test(TestSort)
target_link_libraries(TestSort calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestSelection_SRCS TestSelection.cpp)
add_executable(TestSelectionNAME sheets-Selection ${TestSelection_SRCS})
add_test(TestSelection TestSelection)
ecm_mark_as_test(TestSelection)
target_link_libraries(TestSelection calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestValue_SRCS TestValue.cpp)
add_executable(TestValueNAME sheets-Value  ${TestValue_SRCS})
add_test(TestValue TestValue)
ecm_mark_as_test(TestValue)
target_link_libraries(TestValue calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestStyleStorage_SRCS TestStyleStorage.cpp)
add_executable(TestStyleStorageNAME sheets-StyleStorage ${TestStyleStorage_SRCS})
add_test(TestStyleStorage TestStyleStorage)
ecm_mark_as_test(TestStyleStorage)
target_link_libraries(TestStyleStorage calligrasheetscommon Qt5::Core Qt5::Test)


########### Function tests ###############

set(TestBitopsFunctions_SRCS TestBitopsFunctions.cpp)
add_executable(TestBitopsFunctionsNAME sheets-BitopsFunctions  ${TestBitopsFunctions_SRCS})
add_test(TestBitopsFunctions TestBitopsFunctions)
ecm_mark_as_test(TestBitopsFunctions)
target_link_libraries(TestBitopsFunctions calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestDatabaseFunctions_SRCS TestDatabaseFunctions.cpp)
add_executable(TestDatabaseFunctionsNAME sheets-DatabaseFunctions  ${TestDatabaseFunctions_SRCS})
add_test(TestDatabaseFunctions TestDatabaseFunctions)
ecm_mark_as_test(TestDatabaseFunctions)
target_link_libraries(TestDatabaseFunctions calligrasheetscommon Qt5::Core Qt5::Test)

########### target TestDatetimeFunctions ###############

set(TestDatetimeFunctions_SRCS TestDatetimeFunctions.cpp)
add_executable(TestDatetimeFunctionsNAME sheets-DatetimeFunctions  ${TestDatetimeFunctions_SRCS})
add_test(TestDatetimeFunctions TestDatetimeFunctions)
ecm_mark_as_test(TestDatetimeFunctions)
target_link_libraries(TestDatetimeFunctions calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestEngineeringFunctions_SRCS TestEngineeringFunctions.cpp)
add_executable(TestEngineeringFunctionsNAME sheets-EngineeringFunctions  ${TestEngineeringFunctions_SRCS})
add_test(TestEngineeringFunctions TestEngineeringFunctions)
ecm_mark_as_test(TestEngineeringFunctions)
target_link_libraries(TestEngineeringFunctions calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestFinancialFunctions_SRCS TestFinancialFunctions.cpp)
add_executable(TestFinancialFunctionsNAME sheets-FinancialFunctions  ${TestFinancialFunctions_SRCS})
add_test(TestFinancialFunctions TestFinancialFunctions)
ecm_mark_as_test(TestFinancialFunctions)
target_link_libraries(TestFinancialFunctions calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestInformationFunctions_SRCS TestInformationFunctions.cpp)
add_executable(TestInformationFunctionsNAME sheets-InformationFunctions  ${TestInformationFunctions_SRCS})
add_test(TestInformationFunctions TestInformationFunctions)
ecm_mark_as_test(TestInformationFunctions)
target_link_libraries(TestInformationFunctions calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestLogicFunctions_SRCS TestLogicFunctions.cpp)
add_executable(TestLogicFunctionsNAME sheets-LogicFunctions  ${TestLogicFunctions_SRCS})
add_test(TestLogicFunctions TestLogicFunctions)
ecm_mark_as_test(TestLogicFunctions)
target_link_libraries(TestLogicFunctions calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestMathFunctions_SRCS TestMathFunctions.cpp)
add_executable(TestMathFunctionsNAME sheets-MathFunctions  ${TestMathFunctions_SRCS})
add_test(TestMathFunctions TestMathFunctions)
ecm_mark_as_test(TestMathFunctions)
target_link_libraries(TestMathFunctions calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestStatisticalFunctions_SRCS TestStatisticalFunctions.cpp)
add_executable(TestStatisticalFunctionsNAME sheets-StatisticalFunctions  ${TestStatisticalFunctions_SRCS})
add_test(TestStatisticalFunctions TestStatisticalFunctions)
ecm_mark_as_test(TestStatisticalFunctions)
target_link_libraries(TestStatisticalFunctions calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestTextFunctions_SRCS TestTextFunctions.cpp)
add_executable(TestTextFunctionsNAME sheets-TextFunctions  ${TestTextFunctions_SRCS})
add_test(TestTextFunctions TestTextFunctions)
ecm_mark_as_test(TestTextFunctions)
target_link_libraries(TestTextFunctions calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestTrigFunctions_SRCS TestTrigFunctions.cpp)
add_executable(TestTrigFunctionsNAME sheets-TrigFunctions  ${TestTrigFunctions_SRCS})
add_test(TestTrigFunctions TestTrigFunctions)
ecm_mark_as_test(TestTrigFunctions)
target_link_libraries(TestTrigFunctions calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestValueFormatter_SRCS TestValueFormatter.cpp)
add_executable(TestValueFormatterNAME sheets-ValueFormatter  ${TestValueFormatter_SRCS})
add_test(TestValueFormatter TestValueFormatter)
ecm_mark_as_test(TestValueFormatter)
macro_add_compile_flags(TestValueFormatter "-DCALLIGRA_SHEETS_UNIT_TEST")
target_link_libraries(TestValueFormatter calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestSheet_SRCS TestSheet.cpp)
add_executable(TestSheetNAME sheets-Sheet  ${TestSheet_SRCS})
add_test(TestSheet TestSheet)
ecm_mark_as_test(TestSheet)
macro_add_compile_flags(TestSheet "-DCALLIGRA_SHEETS_UNIT_TEST")
target_link_libraries(TestSheet calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestGraphicsView_SRCS TestGraphicsView.cpp)
add_executable(TestGraphicsView TEST ${TestGraphicsView_SRCS})
target_link_libraries(TestGraphicsView calligrasheetscommon)

########### next target ###############

set(TestRowRepeatStorage_SRCS TestRowRepeatStorage.cpp)
add_executable(TestRowRepeatStorageNAME sheets-RowRepeatStorage ${TestRowRepeatStorage_SRCS})
add_test(TestRowRepeatStorage TestRowRepeatStorage)
ecm_mark_as_test(TestRowRepeatStorage)
target_link_libraries(TestRowRepeatStorage calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestRowFormatStorage_SRCS TestRowFormatStorage.cpp)
add_executable(TestRowFormatStorageNAME sheets-RowFormatStorage ${TestRowFormatStorage_SRCS})
add_test(TestRowFormatStorage TestRowFormatStorage)
ecm_mark_as_test(TestRowFormatStorage)
target_link_libraries(TestRowFormatStorage calligrasheetscommon Qt5::Core Qt5::Test)

########### next target ###############

set(TestDatabaseFilter_SRCS TestDatabaseFilter.cpp)
add_executable(TestDatabaseFilterNAME sheets-Database-Filter ${TestDatabaseFilter_SRCS})
add_test(TestDatabaseFilter TestDatabaseFilter)
ecm_mark_as_test(TestDatabaseFilter)
target_link_libraries(TestDatabaseFilter calligrasheetsodf Qt5::Core Qt5::Test)

########### next target ###############

set(TestPasteCommand_SRCS TestPasteCommand.cpp)
add_executable(TestPasteCommandNAME sheets-PasteCommand ${TestPasteCommand_SRCS})
add_test(TestPasteCommand TestPasteCommand)
ecm_mark_as_test(TestPasteCommand)
target_link_libraries(TestPasteCommand calligrasheetscommon Qt5::Core Qt5::Test)

########### Benchmarks ###############

# set(BenchmarkCluster_SRCS BenchmarkCluster.cpp ../Cluster.cpp) # explicit Cluster.cpp for no extra symbol visibility
# add_executable(BenchmarkCluster TEST ${BenchmarkCluster_SRCS})
# target_link_libraries(BenchmarkCluster calligrasheetscommon Qt5::Test)

########### next target ###############

set(BenchmarkPointStorage_SRCS BenchmarkPointStorage.cpp)
add_executable(BenchmarkPointStorage TEST ${BenchmarkPointStorage_SRCS})
target_link_libraries(BenchmarkPointStorage Qt5::Core Qt5::Test)

########### next target ###############

set(BenchmarkRTree_SRCS BenchmarkRTree.cpp)
add_executable(BenchmarkRTree TEST ${BenchmarkRTree_SRCS})
target_link_libraries(BenchmarkRTree  Qt5::Test)
