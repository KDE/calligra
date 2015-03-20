set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories(
    ..
    ${CMAKE_SOURCE_DIR}/libs/widgetutils
    ${KOODF_INCLUDES}
    ${KUNDO2_INCLUDES}
)

########### next target ###############

set(DateTimeTester_SRCS DateTimeTester.cpp )
 
 
add_executable(DateTimeTesterNAME Plan-DateTimeTester  ${DateTimeTester_SRCS})
add_test(DateTimeTester DateTimeTester)
ecm_mark_as_test(DateTimeTester)
 
target_link_libraries(DateTimeTester   kplatokernel Qt5::Test)
 

########## next target ###############

set(DurationTester_SRCS DurationTester.cpp )
 
 
add_executable(DurationTesterNAME Plan-DurationTester  ${DurationTester_SRCS})
add_test(DurationTester DurationTester)
ecm_mark_as_test(DurationTester)
 
target_link_libraries(DurationTester   kplatokernel Qt5::Test)
 
########## next target ###############

set(AppointmentIntervalTester_SRCS AppointmentIntervalTester.cpp )
 

add_executable(AppointmentIntervalTesterNAME Plan-AppointmentIntervalTester ${AppointmentIntervalTester_SRCS})
add_test(AppointmentIntervalTester AppointmentIntervalTester)
ecm_mark_as_test(AppointmentIntervalTester)
 
target_link_libraries(AppointmentIntervalTester   kplatokernel Qt5::Test)
 
########## next target ###############

set(EstimateTester_SRCS EstimateTester.cpp )
 
 
add_executable(EstimateTesterNAME Plan-EstimateTester  ${EstimateTester_SRCS})
add_test(EstimateTester EstimateTester)
ecm_mark_as_test(EstimateTester)
 
target_link_libraries(EstimateTester   kplatokernel Qt5::Test)
 

########### next target ###############

set(CalendarTester_SRCS CalendarTester.cpp )
 
 
add_executable(CalendarTesterNAME Plan-CalendarTester  ${CalendarTester_SRCS})
add_test(CalendarTester CalendarTester)
ecm_mark_as_test(CalendarTester)
 
target_link_libraries(CalendarTester   kplatokernel Qt5::Test)
 
########## next target ###############

set(ResourceTester_SRCS ResourceTester.cpp )
 

add_executable(ResourceTesterNAME Plan-ResourceTester ${ResourceTester_SRCS})
add_test(ResourceTester ResourceTester)
ecm_mark_as_test(ResourceTester)
 
target_link_libraries(ResourceTester   kplatokernel Qt5::Gui Qt5::Test)
target_link_libraries(ResourceTester kundo2 koodf)

########### next target ###############

set(ScheduleTester_SRCS ScheduleTester.cpp )
 
 
add_executable(ScheduleTesterNAME Plan-ScheduleTester  ${ScheduleTester_SRCS})
add_test(ScheduleTester ScheduleTester)
ecm_mark_as_test(ScheduleTester)
 
target_link_libraries(ScheduleTester   kplatokernel  Qt5::Test)
 

########### next target ###############

set(ProjectTester_SRCS ProjectTester.cpp )
 
 
add_executable(ProjectTesterNAME Plan-ProjectTester  ${ProjectTester_SRCS})
add_test(ProjectTester ProjectTester)
ecm_mark_as_test(ProjectTester)
 
target_link_libraries(ProjectTester   planprivate kplatokernel kundo2 Qt5::Gui Qt5::Test)
 

########### next target ###############

set(AccountTester_SRCS AccountsTester.cpp )
 
 
add_executable(AccountsTesterNAME Plan-AccountsTester  ${AccountTester_SRCS})
add_test(AccountsTester AccountsTester)
ecm_mark_as_test(AccountsTester)
 
target_link_libraries(AccountsTester   planprivate kplatokernel Qt5::Test)
 

########### next target ###############

set(PerformanceTester_SRCS PerformanceTester.cpp )
 
add_executable(PerformanceTesterNAME Plan-PerformanceTester  ${PerformanceTester_SRCS})
add_test(PerformanceTester PerformanceTester)
ecm_mark_as_test(PerformanceTester)
 
target_link_libraries(PerformanceTester   planprivate kplatokernel kundo2 Qt5::Gui Qt5::Test)

########### next target ###############

set(CommandsTester_SRCS CommandsTester.cpp )

add_executable(CommandsTesterNAME Plan-CommandsTester  ${CommandsTester_SRCS})
add_test(CommandsTester CommandsTester)
ecm_mark_as_test(CommandsTester)

target_link_libraries(CommandsTester   planprivate kplatokernel kundo2 Qt5::Gui Qt5::Test)

########### next target ###############

set(WorkInfoCacheTester_SRCS WorkInfoCacheTester.cpp )

add_executable(WorkInfoCacheTesterNAME Plan-WorkInfoCacheTester  ${WorkInfoCacheTester_SRCS})
add_test(WorkInfoCacheTester WorkInfoCacheTester)
ecm_mark_as_test(WorkInfoCacheTester)

target_link_libraries(WorkInfoCacheTester   planprivate kplatokernel Qt5::Gui Qt5::Test)
