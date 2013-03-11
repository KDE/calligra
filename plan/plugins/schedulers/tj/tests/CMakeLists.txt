set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ../taskjuggler ${KPLATO_INCLUDES} ${KOSTORE_INCLUDES})


########### next target ###############

set(TJTester_SRCS TaskJuggler.cpp)
 
 
kde4_add_unit_test(TJTester TESTNAME Plan-TJTester  ${TJTester_SRCS})
 
target_link_libraries(TJTester  ${KDE4_KDECORE_LIBS} plantjscheduler planprivate kplatokernel  ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
 

########### next target ###############

set(SchedulerTester_SRCS SchedulerTester.cpp)


kde4_add_unit_test(SchedulerTester TESTNAME Plan-TJ-SchedulerTester  ${SchedulerTester_SRCS})

target_link_libraries(SchedulerTester  ${KDE4_KDECORE_LIBS} plantjscheduler planprivate kplatokernel koodf  ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
