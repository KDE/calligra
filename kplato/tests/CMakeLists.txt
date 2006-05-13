




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
