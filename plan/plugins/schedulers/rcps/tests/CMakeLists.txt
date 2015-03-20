set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ${KPLATO_INCLUDES} ${KOSTORE_INCLUDES})


########### next target ###############

set(ProjectTester_SRCS ProjectTester.cpp ../KPlatoRCPSScheduler.cpp ../KPlatoRCPSPlugin.cpp)
 
 
add_executable(ProjectTesterNAME Plan-RCPSTester  ${ProjectTester_SRCS})
add_test(ProjectTester ProjectTester)
ecm_mark_as_test(ProjectTester)
 
target_link_libraries(ProjectTester   rcps_plan planprivate kplatokernel  Qt5::Gui Qt5::Test)
 

########### next target ###############
