set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ../taskjuggler ${KPLATO_INCLUDES} ${KOSTORE_INCLUDES})

# call: planschedulers_tj_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(PLANSCHEDULERS_TJ_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "plan-schedulers-tj-"
    )
endmacro()

########### next target ###############

planschedulers_tj_add_unit_test(TJTester
    TaskJuggler.cpp
    LINK_LIBRARIES plantjscheduler planprivate kplatokernel Qt5::Test
)

########### next target ###############

planschedulers_tj_add_unit_test(SchedulerTester
    SchedulerTester.cpp
    LINK_LIBRARIES plantjscheduler planprivate kplatokernel planodf Qt5::Test
)
