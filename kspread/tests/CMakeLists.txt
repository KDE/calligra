if(KDE4_BUILD_TESTS)

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KOFFICEUI_INCLUDES} )

########### next target ###############

set(TestFormula_SRCS TestFormula.cpp)

kde4_automoc(${TestFormula_SRCS})

kde4_add_executable(TestFormula RUN_UNINSTALLED ${TestFormula_SRCS})

target_link_libraries(TestFormula kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(kspread-Formula TestFormula)

########### next target ###############

set(TestOpenFormula_SRCS TestOpenFormula.cpp)

kde4_automoc(${TestOpenFormula_SRCS})

kde4_add_executable(TestOpenFormula RUN_UNINSTALLED ${TestOpenFormula_SRCS})

target_link_libraries(TestOpenFormula kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(kspread-OpenFormula TestOpenFormula)

########### next target ###############

set(BenchmarkRTree_SRCS BenchmarkRTree.cpp)

kde4_automoc(${BenchmarkRTree_SRCS})

kde4_add_executable(BenchmarkRTree RUN_UNINSTALLED ${BenchmarkRTree_SRCS})

target_link_libraries(BenchmarkRTree ${KDE4_KDECORE_LIBS})

# add_test(kspread-BenchmarkRTree BenchmarkRTreetest)

########### next target ###############

set(TestRTree_SRCS TestRTree.cpp)

kde4_automoc(${TestRTree_SRCS})

kde4_add_executable(TestRTree RUN_UNINSTALLED ${TestRTree_SRCS})

target_link_libraries(TestRTree ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})

add_test(kpsread-RTree TestRTree)

########### next target ###############

set(TestValue_SRCS TestValue.cpp)

kde4_automoc(${TestValue_SRCS})

kde4_add_executable(TestValue RUN_UNINSTALLED ${TestValue_SRCS})

target_link_libraries(TestValue kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(kspread-Value TestValue)

########### next target ###############

set(TestMathFunctions_SRCS TestMathFunctions.cpp)

kde4_automoc(${TestMathFunctions_SRCS})

kde4_add_executable(TestMathFunctions RUN_UNINSTALLED ${TestMathFunctions_SRCS})

target_link_libraries(TestMathFunctions kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(kspread-MathFunctions TestMathFunctions)

########### next target ###############

set(TestFinancialFunctions_SRCS TestFinancialFunctions.cpp)

kde4_automoc(${TestFinancialFunctions_SRCS})

kde4_add_executable(TestFinancialFunctions RUN_UNINSTALLED ${TestFinancialFunctions_SRCS})

target_link_libraries(TestFinancialFunctions kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(kspread-FinancialFunctions TestFinancialFunctions)

########### next target ###############

set(TestTrigFunctions_SRCS TestTrigFunctions.cpp)

kde4_automoc(${TestTrigFunctions_SRCS})

kde4_add_executable(TestTrigFunctions RUN_UNINSTALLED ${TestTrigFunctions_SRCS})

target_link_libraries(TestTrigFunctions kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(kspread-TrigFunctions TestTrigFunctions)

########### next target ###############

set(BenchmarkCluster_SRCS BenchmarkCluster.cpp ../Cluster.cpp) # explicit Cluster.cpp for no extra symbol visibility

kde4_automoc(${BenchmarkCluster_SRCS})

kde4_add_executable(BenchmarkCluster RUN_UNINSTALLED ${BenchmarkCluster_SRCS})

target_link_libraries(BenchmarkCluster kspreadcommon ${QT_QTTEST_LIBRARY})

#add_test(kspread-BenchmarkCluster BenchmarkCluster)

########### next target ###############

set(BenchmarkPointStorage_SRCS BenchmarkPointStorage.cpp)

kde4_automoc(${BenchmarkPointStorage_SRCS})

kde4_add_executable(BenchmarkPointStorage RUN_UNINSTALLED ${BenchmarkPointStorage_SRCS})

target_link_libraries(BenchmarkPointStorage ${QT_QTTEST_LIBRARY})

# add_test(kspread-BenchmarkPointStorage BenchmarkPointStoragetest)

########### next target ###############

set(TestPointStorage_SRCS TestPointStorage.cpp)

kde4_automoc(${TestPointStorage_SRCS})

kde4_add_executable(TestPointStorage RUN_UNINSTALLED ${TestPointStorage_SRCS})

target_link_libraries(TestPointStorage kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(kspread-PointStorage TestPointStorage)

########### next target ###############

set(TestBitopsFunctions_SRCS TestBitopsFunctions.cpp)

kde4_automoc(${TestBitopsFunctions_SRCS})

kde4_add_executable(TestBitopsFunctions RUN_UNINSTALLED ${TestBitopsFunctions_SRCS})

target_link_libraries(TestBitopsFunctions kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(kspread-BitopsFunctions TestBitopsFunctions)

########### next target ###############

set(TestInformationFunctions_SRCS TestInformationFunctions.cpp)

kde4_automoc(${TestInformationFunctions_SRCS})

kde4_add_executable(TestInformationFunctions RUN_UNINSTALLED ${TestInformationFunctions_SRCS})

target_link_libraries(TestInformationFunctions kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(kspread-InformationFunctions TestInformationFunctions)

########### next target ###############

set(TestEngineeringFunctions_SRCS TestEngineeringFunctions.cpp)

kde4_automoc(${TestEngineeringFunctions_SRCS})

kde4_add_executable(TestEngineeringFunctions RUN_UNINSTALLED ${TestEngineeringFunctions_SRCS})

target_link_libraries(TestEngineeringFunctions kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(kspread-EngineeringFunctions TestEngineeringFunctions)

########### next target ###############

set(TestLogicFunctions_SRCS TestLogicFunctions.cpp)

kde4_automoc(${TestLogicFunctions_SRCS})

kde4_add_executable(TestLogicFunctions RUN_UNINSTALLED ${TestLogicFunctions_SRCS})

target_link_libraries(TestLogicFunctions kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(kspread-LogicFunctions TestLogicFunctions)

########### next target ###############

set(TestStatisticalFunctions_SRCS TestStatisticalFunctions.cpp)

kde4_automoc(${TestStatisticalFunctions_SRCS})

kde4_add_executable(TestStatisticalFunctions RUN_UNINSTALLED ${TestStatisticalFunctions_SRCS})

target_link_libraries(TestStatisticalFunctions kspreadcommon ${QT_QTTEST_LIBRARY})

add_test(kspread-StatisticalFunctions TestStatisticalFunctions)


endif(KDE4_BUILD_TESTS)
