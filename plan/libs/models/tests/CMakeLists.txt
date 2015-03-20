set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ../../kernel ${KOODF_INCLUDES} ${KUNDO2_INCLUDES})

########### next target ###############

set(ResourceModelTester_SRCS ResourceModelTester.cpp )
 
 
add_executable(ResourceModelTesterNAME Plan-ResourceModelTester  ${ResourceModelTester_SRCS})
add_test(ResourceModelTester ResourceModelTester)
ecm_mark_as_test(ResourceModelTester)
 
target_link_libraries(ResourceModelTester   kplatomodels kplatokernel Qt5::Test)
 

########## next target ###############

set(FlatProxyModelTester_SRCS FlatProxyModelTester.cpp)
 
 
add_executable(FlatProxyModelTesterNAME Plan-FlatProxyModelTester  ${FlatProxyModelTester_SRCS})
add_test(FlatProxyModelTester FlatProxyModelTester)
ecm_mark_as_test(FlatProxyModelTester)
 
target_link_libraries(FlatProxyModelTester   kplatomodels Qt5::Test)
 

########## next target ###############

set(WorkPackageProxyModelTester_SRCS WorkPackageProxyModelTester.cpp)
 
 
add_executable(WorkPackageProxyModelTesterNAME Plan-WorkPackageProxyModelTester  ${WorkPackageProxyModelTester_SRCS})
add_test(WorkPackageProxyModelTester WorkPackageProxyModelTester)
ecm_mark_as_test(WorkPackageProxyModelTester)
 
target_link_libraries(WorkPackageProxyModelTester   kplatomodels kplatokernel kundo2 Qt5::Test)
 

########## next target ###############
