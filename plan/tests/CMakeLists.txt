set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ${KPLATO_INCLUDES} ${KOODF_INCLUDES})

########### test files ###############

set(test_files version-0-6.kplato project_stats1.kplato pert1.kplato)
foreach(it ${test_files})
    ADD_CUSTOM_TARGET(${it} ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${it} ${CMAKE_CURRENT_BINARY_DIR}/${it}
        COMMENT "Copy test file ${it}" VERBATIM
    )
endforeach(it)
 
########### next target ###############

# NOTE: .kplato files is no longer native, so need some magic to run this test 
# set(InsertFileTester_SRCS InsertFileTester.cpp)
#  
# kde4_add_unit_test(InsertFileTester TESTNAME Plan-InsertFileTester  ${InsertFileTester_SRCS})
#  
# target_link_libraries(InsertFileTester  planprivate kplatokernel komain ${KDE4_KDEUI_LIBS} ${QT_QTTEST_LIBRARY})
 

########## next target ###############

set(InsertProjectTester_SRCS InsertProjectTester.cpp)
 
kde4_add_unit_test(InsertProjectTester TESTNAME Plan-InsertProjectTester  ${InsertProjectTester_SRCS})

target_link_libraries(InsertProjectTester  planprivate kplatokernel komain ${KDE4_KDEUI_LIBS} ${QT_QTTEST_LIBRARY})

########## next target ###############
