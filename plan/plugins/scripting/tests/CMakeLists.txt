set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ${KPLATO_INCLUDES} ${KOKROSS_INCLUDES} ${KOSTORE_INCLUDES})

add_definitions(-DFILES_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

########### next target ###############

set (ScriptingTester_SRCS ScriptingTester.cpp TestResult.cpp)

ecm_add_test(
    ${ScriptingTester_SRCS}
    TEST_NAME ScriptingTester
    NAME_PREFIX "plan-"
    LINK_LIBRARIES
        krossmoduleplan
        planprivate
        kplatokernel
        kokross
        Qt5::Test
)
