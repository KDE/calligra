set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ../taskjuggler ${KPLATO_INCLUDES} ${KOSTORE_INCLUDES})


########### next target ###############

set(TJTester_SRCS TaskJuggler.cpp)
 
 
add_executable(TJTesterNAME Plan-TJTester  ${TJTester_SRCS})
add_test(TJTester TJTester)
ecm_mark_as_test(TJTester)
 
target_link_libraries(TJTester   plantjscheduler planprivate kplatokernel  Qt5::Core Qt5::Test)
 

########### next target ###############

set(SchedulerTester_SRCS SchedulerTester.cpp)


add_executable(SchedulerTesterNAME Plan-TJ-SchedulerTester  ${SchedulerTester_SRCS})
add_test(SchedulerTester SchedulerTester)
ecm_mark_as_test(SchedulerTester)

target_link_libraries(SchedulerTester   plantjscheduler planprivate kplatokernel koodf  Qt5::Core Qt5::Test)
