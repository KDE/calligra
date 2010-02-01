set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ../../kernel ${KOODF_INCLUDES})

########### next target ###############

set(ResourceModelTester_SRCS ResourceModelTester.cpp )
 
 
kde4_add_unit_test(ResourceModelTester TESTNAME KPlato-ResourceModelTester  ${ResourceModelTester_SRCS})
 
target_link_libraries(ResourceModelTester  ${KDE4_KDECORE_LIBS} kplatomodels kplatokernel ${QT_QTTEST_LIBRARY})
 

########## next target ###############

set(FlatProxyModelTester_SRCS FlatProxyModelTester.cpp)
 
 
kde4_add_unit_test(FlatProxyModelTester TESTNAME KPlato-FlatProxyModelTester  ${FlatProxyModelTester_SRCS})
 
target_link_libraries(FlatProxyModelTester  ${KDE4_KDEUI_LIBS} kplatomodels ${QT_QTTEST_LIBRARY})
 

########## next target ###############

