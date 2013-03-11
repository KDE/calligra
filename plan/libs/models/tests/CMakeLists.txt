set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ../../kernel ${KOODF_INCLUDES} ${KUNDO2_INCLUDES})

########### next target ###############

set(ResourceModelTester_SRCS ResourceModelTester.cpp )
 
 
kde4_add_unit_test(ResourceModelTester TESTNAME Plan-ResourceModelTester  ${ResourceModelTester_SRCS})
 
target_link_libraries(ResourceModelTester  ${KDE4_KDECORE_LIBS} kplatomodels kplatokernel ${QT_QTTEST_LIBRARY})
 

########## next target ###############

set(FlatProxyModelTester_SRCS FlatProxyModelTester.cpp)
 
 
kde4_add_unit_test(FlatProxyModelTester TESTNAME Plan-FlatProxyModelTester  ${FlatProxyModelTester_SRCS})
 
target_link_libraries(FlatProxyModelTester  ${KDE4_KDEUI_LIBS} kplatomodels ${QT_QTTEST_LIBRARY})
 

########## next target ###############

set(WorkPackageProxyModelTester_SRCS WorkPackageProxyModelTester.cpp)
 
 
kde4_add_unit_test(WorkPackageProxyModelTester TESTNAME Plan-WorkPackageProxyModelTester  ${WorkPackageProxyModelTester_SRCS})
 
target_link_libraries(WorkPackageProxyModelTester  ${KDE4_KDEUI_LIBS} kplatomodels kplatokernel kundo2 ${QT_QTTEST_LIBRARY})
 

########## next target ###############
