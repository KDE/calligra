set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ${KPLATO_INCLUDES} ${KOODF_INCLUDES})

########### test files ###############

set(test_files version-0-6.kplato project_stats1.kplato pert1.kplato)
foreach(it ${test_files})
    add_custom_target(${it} ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${it} ${CMAKE_CURRENT_BINARY_DIR}/${it}
        COMMENT "Copy test file ${it}" VERBATIM
    )
endforeach(it)
 
########### next target ###############

# NOTE: .kplato files is no longer native, so need some magic to run this test 
# set(InsertFileTester_SRCS InsertFileTester.cpp)
#  
# add_executable(InsertFileTesterNAME Plan-InsertFileTester  ${InsertFileTester_SRCS})
add_test(InsertFileTester InsertFileTester)
ecm_mark_as_test(InsertFileTester)
#  
# target_link_libraries(InsertFileTester  planprivate kplatokernel komain  Qt5::Test)
 

########## next target ###############

set(InsertProjectTester_SRCS InsertProjectTester.cpp)
 
add_executable(InsertProjectTesterNAME Plan-InsertProjectTester  ${InsertProjectTester_SRCS})
add_test(InsertProjectTester InsertProjectTester)
ecm_mark_as_test(InsertProjectTester)

target_link_libraries(InsertProjectTester  planprivate kplatokernel komain  Qt5::Test)

########## next target ###############
