set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ${KPLATO_INCLUDES} ${KOSTORE_INCLUDES})

########### next target ###############

set(InsertFileTester_SRCS InsertFileTester.cpp )
 
 
kde4_add_unit_test(InsertFileTester TESTNAME KPlato-InsertFileTester  ${InsertFileTester_SRCS})
 
target_link_libraries(InsertFileTester  kplatoprivate ${QT_QTTEST_LIBRARY})
 

########## next target ###############

