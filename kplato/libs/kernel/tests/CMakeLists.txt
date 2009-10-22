set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ${KOSTORE_INCLUDES})

########### next target ###############

set(DateTimeTester_SRCS DateTimeTester.cpp )
 
 
kde4_add_unit_test(DateTimeTester TESTNAME KPlato-DateTimeTester  ${DateTimeTester_SRCS})
 
target_link_libraries(DateTimeTester  ${KDE4_KDECORE_LIBS} kplatokernel ${QT_QTTEST_LIBRARY})
 

########## next target ###############

set(DurationTester_SRCS DurationTester.cpp )
 
 
kde4_add_unit_test(DurationTester TESTNAME KPlato-DurationTester  ${DurationTester_SRCS})
 
target_link_libraries(DurationTester  ${KDE4_KDECORE_LIBS} kplatokernel ${QT_QTTEST_LIBRARY})
 
########## next target ###############

set(AppointmentIntervalTester_SRCS AppointmentIntervalTester.cpp )
 

kde4_add_unit_test(AppointmentIntervalTester TESTNAME KPlato-AppointmentIntervalTester ${AppointmentIntervalTester_SRCS})
 
target_link_libraries(AppointmentIntervalTester  ${KDE4_KDECORE_LIBS} kplatokernel ${QT_QTTEST_LIBRARY})
 
########## next target ###############

set(EstimateTester_SRCS EstimateTester.cpp )
 
 
kde4_add_unit_test(EstimateTester TESTNAME KPlato-EstimateTester  ${EstimateTester_SRCS})
 
target_link_libraries(EstimateTester  ${KDE4_KDECORE_LIBS} kplatokernel ${QT_QTTEST_LIBRARY})
 

########### next target ###############

set(CalendarTester_SRCS CalendarTester.cpp )
 
 
kde4_add_unit_test(CalendarTester TESTNAME KPlato-CalendarTester  ${CalendarTester_SRCS})
 
target_link_libraries(CalendarTester  ${KDE4_KDECORE_LIBS} kplatokernel ${QT_QTTEST_LIBRARY})
 

########### next target ###############

set(ScheduleTester_SRCS ScheduleTester.cpp )
 
 
kde4_add_unit_test(ScheduleTester TESTNAME KPlato-ScheduleTester  ${ScheduleTester_SRCS})
 
target_link_libraries(ScheduleTester  ${KDE4_KDECORE_LIBS} kplatokernel ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
 

########### next target ###############

set(ProjectTester_SRCS ProjectTester.cpp )
 
 
kde4_add_unit_test(ProjectTester TESTNAME KPlato-ProjectTester  ${ProjectTester_SRCS})
 
target_link_libraries(ProjectTester  ${KDE4_KDECORE_LIBS} kplatoprivate kplatokernel ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY})
 

########### next target ###############

set(AccountTester_SRCS AccountsTester.cpp )
 
 
kde4_add_unit_test(AccountsTester TESTNAME KPlato-AccountsTester  ${AccountTester_SRCS})
 
target_link_libraries(AccountsTester  ${KDE4_KDECORE_LIBS} kplatoprivate kplatokernel ${QT_QTTEST_LIBRARY})
 

########### next target ###############
