set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ${KPLATO_INCLUDES} ${KOSTORE_INCLUDES})

########### test files ###############

set(test_files version-0-6.kplato project_stats1.kplato pert1.kplato)
foreach(it ${test_files})
    ADD_CUSTOM_TARGET(${it} ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${it} ${CMAKE_CURRENT_BINARY_DIR}/${it}
        COMMENT "Copy test file ${it}" VERBATIM
    )
endforeach(it)
 
########### next target ###############

set(InsertFileTester_SRCS InsertFileTester.cpp)
 
kde4_add_unit_test(InsertFileTester TESTNAME KPlato-InsertFileTester  ${InsertFileTester_SRCS})
 
target_link_libraries(InsertFileTester  kplatoprivate ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
 

########## next target ###############

set(InsertProjectTester_SRCS InsertProjectTester.cpp)
 
kde4_add_unit_test(InsertProjectTester TESTNAME KPlato-InsertProjectTester  ${InsertProjectTester_SRCS})

target_link_libraries(InsertProjectTester  kplatoprivate ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})

########## next target ###############
