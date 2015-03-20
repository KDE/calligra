set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ${KPLATO_INCLUDES} ${KOKROSS_INCLUDES} ${KOSTORE_INCLUDES})

add_definitions(-DFILES_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

########### next target ###############

set (ScriptingTester_SRCS ScriptingTester.cpp TestResult.cpp)

add_executable(ScriptingTesterNAME Plan-ScriptingTester  ${ScriptingTester_SRCS})
add_test(ScriptingTester ScriptingTester)
ecm_mark_as_test(ScriptingTester)

target_link_libraries(ScriptingTester 
        krossmoduleplan
        planprivate
        kplatokernel
        kokross
        
        Qt5::Gui
        Qt5::Test
)


########### next target ###############
