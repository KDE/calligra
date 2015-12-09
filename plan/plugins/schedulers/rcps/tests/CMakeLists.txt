set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ${KPLATO_INCLUDES} ${KOSTORE_INCLUDES})


########### next target ###############

set(ProjectTester_SRCS ProjectTester.cpp ../KPlatoRCPSScheduler.cpp ../KPlatoRCPSPlugin.cpp)
 
add_definitions(-DPLAN_NOPLUGIN)

ecm_add_test( ${ProjectTester_SRCS}
    TEST_NAME "RCPSTester"
    NAME_PREFIX "plan-schedulers-rcps-"
    LINK_LIBRARIES rcps_plan planprivate kplatokernel Qt5::Test
)
