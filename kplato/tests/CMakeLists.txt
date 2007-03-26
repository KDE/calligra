
if (KDE4_BUILD_TESTS)

include_directories( .. ${KPLATO_INCLUDES})

########### next target ###############

set(DateTimeTester_SRCS DateTimeTester.cpp )
 
kde4_automoc(${DateTimeTester_SRCS})
 
kde4_add_executable(DateTimeTester NOGUI RUN_UNINSTALLED ${DateTimeTester_SRCS})
 
target_link_libraries(DateTimeTester  ${KDE4_KDECORE_LIBS} kplatoprivate ${QT_QTTEST_LIBRARY})
 
add_test(KPlato-DateTimeTester  ${EXECUTABLE_OUTPUT_PATH}/DateTimeTester)

########### next target ###############

set(DurationTester_SRCS DurationTester.cpp )
 
kde4_automoc(${DurationTester_SRCS})
 
kde4_add_executable(DurationTester NOGUI RUN_UNINSTALLED ${DurationTester_SRCS})
 
target_link_libraries(DurationTester  ${KDE4_KDECORE_LIBS} kplatoprivate ${QT_QTTEST_LIBRARY})
 
add_test(KPlato-DurationTester  ${EXECUTABLE_OUTPUT_PATH}/DurationTester)

########### next target ###############

set(CalendarTester_SRCS CalendarTester.cpp )
 
kde4_automoc(${CalendarTester_SRCS})
 
kde4_add_executable(CalendarTester NOGUI RUN_UNINSTALLED ${CalendarTester_SRCS})
 
target_link_libraries(CalendarTester  ${KDE4_KDECORE_LIBS} kplatoprivate ${QT_QTTEST_LIBRARY})
 
add_test(KPlato-CalendarTester  ${EXECUTABLE_OUTPUT_PATH}/CalendarTester)

########### next target ###############

set(ProjectTester_SRCS ProjectTester.cpp )
 
kde4_automoc(${ProjectTester_SRCS})
 
kde4_add_executable(ProjectTester NOGUI RUN_UNINSTALLED ${ProjectTester_SRCS})
 
target_link_libraries(ProjectTester  ${KDE4_KDECORE_LIBS} kplatoprivate ${QT_QTTEST_LIBRARY})
 
add_test(KPlato-ProjectTester  ${EXECUTABLE_OUTPUT_PATH}/ProjectTester)

########### next target ###############


endif(KDE4_BUILD_TESTS)
