set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ${KPLATO_INCLUDES} ${KOKROSS_INCLUDES} ${KOSTORE_INCLUDES})

add_definitions(-DFILES_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

########### next target ###############

set (ScriptingTester_SRCS ScriptingTester.cpp TestResult.cpp)

kde4_add_unit_test(ScriptingTester TESTNAME Plan-ScriptingTester  ${ScriptingTester_SRCS})

target_link_libraries(ScriptingTester 
        krossmoduleplan
        planprivate
        kplatokernel
        kokross
        
        Qt5::Gui
        Qt5::Test
)


########### next target ###############
