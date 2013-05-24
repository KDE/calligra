set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ${KOODF_INCLUDES} ${KUNDO2_INCLUDES} ${KOMAIN_INCLUDES})

########### next target ###############

set(DateTimeTester_SRCS DateTimeTester.cpp )
 
 
kde4_add_unit_test(DateTimeTester TESTNAME Plan-DateTimeTester  ${DateTimeTester_SRCS})
 
target_link_libraries(DateTimeTester  ${KDE4_KDECORE_LIBS} kplatokernel ${QT_QTTEST_LIBRARY})
 

########## next target ###############

set(DurationTester_SRCS DurationTester.cpp )
 
 
kde4_add_unit_test(DurationTester TESTNAME Plan-DurationTester  ${DurationTester_SRCS})
 
target_link_libraries(DurationTester  ${KDE4_KDECORE_LIBS} kplatokernel ${QT_QTTEST_LIBRARY})
 
########## next target ###############

set(AppointmentIntervalTester_SRCS AppointmentIntervalTester.cpp )
 

kde4_add_unit_test(AppointmentIntervalTester TESTNAME Plan-AppointmentIntervalTester ${AppointmentIntervalTester_SRCS})
 
target_link_libraries(AppointmentIntervalTester  ${KDE4_KDECORE_LIBS} kplatokernel ${QT_QTTEST_LIBRARY})
 
########## next target ###############

set(EstimateTester_SRCS EstimateTester.cpp )
 
 
kde4_add_unit_test(EstimateTester TESTNAME Plan-EstimateTester  ${EstimateTester_SRCS})
 
target_link_libraries(EstimateTester  ${KDE4_KDECORE_LIBS} kplatokernel ${QT_QTTEST_LIBRARY})
 

########### next target ###############

set(CalendarTester_SRCS CalendarTester.cpp )
 
 
kde4_add_unit_test(CalendarTester TESTNAME Plan-CalendarTester  ${CalendarTester_SRCS})
 
target_link_libraries(CalendarTester  ${KDE4_KDECORE_LIBS} kplatokernel ${QT_QTTEST_LIBRARY})
 
########## next target ###############

set(ResourceTester_SRCS ResourceTester.cpp )
 

kde4_add_unit_test(ResourceTester TESTNAME Plan-ResourceTester ${ResourceTester_SRCS})
 
target_link_libraries(ResourceTester  ${KDE4_KDECORE_LIBS} kplatokernel ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY})
target_link_libraries(ResourceTester kundo2 koodf)

########### next target ###############

set(ScheduleTester_SRCS ScheduleTester.cpp )
 
 
kde4_add_unit_test(ScheduleTester TESTNAME Plan-ScheduleTester  ${ScheduleTester_SRCS})
 
target_link_libraries(ScheduleTester  ${KDE4_KDECORE_LIBS} kplatokernel ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
 

########### next target ###############

set(ProjectTester_SRCS ProjectTester.cpp )
 
 
kde4_add_unit_test(ProjectTester TESTNAME Plan-ProjectTester  ${ProjectTester_SRCS})
 
target_link_libraries(ProjectTester  ${KDE4_KDECORE_LIBS} planprivate kplatokernel kundo2 ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY})
 

########### next target ###############

set(AccountTester_SRCS AccountsTester.cpp )
 
 
kde4_add_unit_test(AccountsTester TESTNAME Plan-AccountsTester  ${AccountTester_SRCS})
 
target_link_libraries(AccountsTester  ${KDE4_KDECORE_LIBS} planprivate kplatokernel ${QT_QTTEST_LIBRARY})
 

########### next target ###############

set(PerformanceTester_SRCS PerformanceTester.cpp )
 
kde4_add_unit_test(PerformanceTester TESTNAME Plan-PerformanceTester  ${PerformanceTester_SRCS})
 
target_link_libraries(PerformanceTester  ${KDE4_KDECORE_LIBS} planprivate kplatokernel kundo2 ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(CommandsTester_SRCS CommandsTester.cpp )

kde4_add_unit_test(CommandsTester TESTNAME Plan-CommandsTester  ${CommandsTester_SRCS})

target_link_libraries(CommandsTester  ${KDE4_KDECORE_LIBS} planprivate kplatokernel kundo2 ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(WorkInfoCacheTester_SRCS WorkInfoCacheTester.cpp )

kde4_add_unit_test(WorkInfoCacheTester TESTNAME Plan-WorkInfoCacheTester  ${WorkInfoCacheTester_SRCS})

target_link_libraries(WorkInfoCacheTester  ${KDE4_KDECORE_LIBS} planprivate kplatokernel ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY})
