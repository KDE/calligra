set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories(
    ${CMAKE_SOURCE_DIR}/sheets
    ${CMAKE_BINARY_DIR}/sheets
    ${EIGEN3_INCLUDE_DIR}
    ${KOMAIN_INCLUDES}
)

# call: sheets_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(SHEETS_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "sheets-"
    )
endmacro()

########### Core Functionality ###############

sheets_add_unit_test(Cell
    TestCell.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(CellStorage
    TestCellStorage.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(Dependencies
    TestDependencies.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(Formula
    TestFormula.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(Util
    TestUtil.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(OpenFormula
    TestOpenFormula.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(PointStorage
    TestPointStorage.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(Region
    TestRegion.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(RTree
    TestRTree.cpp
    LINK_LIBRARIES calligrasheetscommon KF5::KDELibs4Support Qt5::Test
)

########### next target ###############

sheets_add_unit_test(Sort
    TestSort.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(Selection
    TestSelection.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(Value
    TestValue.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(StyleStorage
    TestStyleStorage.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)


########### Function tests ###############

sheets_add_unit_test(BitopsFunctions
    TestBitopsFunctions.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(DatabaseFunctions
    TestDatabaseFunctions.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### target TestDatetimeFunctions ###############

sheets_add_unit_test(DatetimeFunctions
    TestDatetimeFunctions.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(EngineeringFunctions
    TestEngineeringFunctions.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(FinancialFunctions
    TestFinancialFunctions.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(InformationFunctions
    TestInformationFunctions.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(LogicFunctions
    TestLogicFunctions.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(MathFunctions
    TestMathFunctions.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(StatisticalFunctions
    TestStatisticalFunctions.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(TextFunctions
    TestTextFunctions.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(TrigFunctions
    TestTrigFunctions.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

# the next tests had this line:
# set_target_properties(TestValueFormatter PROPERTIES COMPILE_FLAGS "-DCALLIGRA_SHEETS_UNIT_TEST")
# but ecm_add_test has no official support for injecting properties or getting the target (ECM v5.17)
# For now just set and unset globally:
add_definitions("-DCALLIGRA_SHEETS_UNIT_TEST")

########### next target ###############

sheets_add_unit_test(ValueFormatter
    TestValueFormatter.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(ValueConverter
    TestValueConverter.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(ValueParser
    TestValueParser.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(Sheet
    TestSheet.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

# see comment at respective add_definitions(...)
remove_definitions("-DCALLIGRA_SHEETS_UNIT_TEST")

########### next target ###############

set(TestGraphicsView_SRCS TestGraphicsView.cpp)
add_executable(TestGraphicsView ${TestGraphicsView_SRCS})
ecm_mark_as_test(TestGraphicsView)
target_link_libraries(TestGraphicsView calligrasheetscommon)

########### next target ###############

sheets_add_unit_test(RowRepeatStorage
    TestRowRepeatStorage.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(RowFormatStorage
    TestRowFormatStorage.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(DatabaseFilter
    TestDatabaseFilter.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### next target ###############

sheets_add_unit_test(PasteCommand
    TestPasteCommand.cpp
    LINK_LIBRARIES calligrasheetscommon Qt5::Test
)

########### Benchmarks ###############

# set(BenchmarkCluster_SRCS BenchmarkCluster.cpp ../Cluster.cpp) # explicit Cluster.cpp for no extra symbol visibility
# add_executable(BenchmarkCluster ${BenchmarkCluster_SRCS})
# ecm_mark_as_test(BenchmarkCluster)
# target_link_libraries(BenchmarkCluster calligrasheetscommon Qt5::Test)

########### next target ###############

set(BenchmarkPointStorage_SRCS BenchmarkPointStorage.cpp)
add_executable(BenchmarkPointStorage ${BenchmarkPointStorage_SRCS})
ecm_mark_as_test(BenchmarkPointStorage)
target_link_libraries(BenchmarkPointStorage Qt5::Test KF5::KDELibs4Support)

########### next target ###############

set(BenchmarkRTree_SRCS BenchmarkRTree.cpp)
add_executable(BenchmarkRTree ${BenchmarkRTree_SRCS})
ecm_mark_as_test(BenchmarkRTree)
target_link_libraries(BenchmarkRTree KF5::KDELibs4Support Qt5::Test)
