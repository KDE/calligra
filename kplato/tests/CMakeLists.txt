
if (KDE4_BUILD_TESTS)

include_directories(${KPLATO_INCLUDES} ${KDE4_INCLUDES})

set(tutorial1_SRCS tutorial1.cc )
 
kde4_automoc(${tutorial1_SRCS})
 
kde4_add_executable(tutorial1 NOGUI RUN_UNINSTALLED ${tutorial1_SRCS})
 
target_link_libraries(tutorial1  ${KDE4_KDECORE_LIBS} kplatopart ${QT_QTTEST_LIBRARY})
 
add_test(kplato-tutorial1  ${EXECUTABLE_OUTPUT_PATH}/tutorial1)




########### install files ###############






#original Makefile.am contents follow:

#AM_CPPFLAGS = -I$(srcdir)/../ $(KOFFICE_INCLUDES) $(all_includes)
#
#INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)
#
## The check_ target makes sure we don't install the modules,
## $(KDE_CHECK_PLUGIN) assures a shared library is created.
#check_LTLIBRARIES = kunittest_DateTimeTester.la \
#                    kunittest_DurationTester.la \
#                    kunittest_CalendarTester.la
#
#kunittest_DateTimeTester_la_SOURCES = DateTimeTester.cpp
#kunittest_DateTimeTester_la_LIBADD = -lkunittest ../libkplatopart.la
#kunittest_DateTimeTester_la_LDFLAGS = -module $(KDE_CHECK_PLUGIN) $(all_libraries)
#
#kunittest_DurationTester_la_SOURCES = DurationTester.cpp
#kunittest_DurationTester_la_LIBADD = -lkunittest ../libkplatopart.la
#kunittest_DurationTester_la_LDFLAGS = -module $(KDE_CHECK_PLUGIN) $(all_libraries)
#
#kunittest_CalendarTester_la_SOURCES = CalendarTester.cpp
#kunittest_CalendarTester_la_LIBADD = -lkunittest ../libkplatopart.la
#kunittest_CalendarTester_la_LDFLAGS = -module $(KDE_CHECK_PLUGIN) $(all_libraries)
#
#check-local: kunittest_DateTimeTester.la \
#             kunittest_DurationTester.la \
#             kunittest_CalendarTester.la
#
#check: 
#	kunittestmodrunner
#

endif(KDE4_BUILD_TESTS)
