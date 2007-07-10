include_directories( .. ${KPLATO_INCLUDES})

########### next target ###############

set(DateTimeTester_SRCS DateTimeTester.cpp )
 
 
kde4_add_unit_test(DateTimeTester TESTNAME KPlato-DateTimeTester  ${DateTimeTester_SRCS})
 
target_link_libraries(DateTimeTester  ${KDE4_KDECORE_LIBS} kplatoprivate ${QT_QTTEST_LIBRARY})
 

########## next target ###############

set(DurationTester_SRCS DurationTester.cpp )
 
 
kde4_add_unit_test(DurationTester TESTNAME KPlato-DurationTester  ${DurationTester_SRCS})
 
target_link_libraries(DurationTester  ${KDE4_KDECORE_LIBS} kplatoprivate ${QT_QTTEST_LIBRARY})
 

########### next target ###############

set(CalendarTester_SRCS CalendarTester.cpp )
 
 
kde4_add_unit_test(CalendarTester TESTNAME KPlato-CalendarTester  ${CalendarTester_SRCS})
 
target_link_libraries(CalendarTester  ${KDE4_KDECORE_LIBS} kplatoprivate ${QT_QTTEST_LIBRARY})
 

########### next target ###############

set(ProjectTester_SRCS ProjectTester.cpp )
 
 
kde4_add_unit_test(ProjectTester TESTNAME KPlato-ProjectTester  ${ProjectTester_SRCS})
 
target_link_libraries(ProjectTester  ${KDE4_KDECORE_LIBS} kplatoprivate ${QT_QTTEST_LIBRARY})
 

########### next target ###############
