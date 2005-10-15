AM_CPPFLAGS = -I$(srcdir)/../ $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) $(all_includes)

# The check_ target makes sure we don't install the modules,
# $(KDE_CHECK_PLUGIN) assures a shared library is created.
check_LTLIBRARIES = kunittest_KWPageManagerTester.la

kunittest_KWPageManagerTester_la_SOURCES = KWPageManagerTester.cpp
kunittest_KWPageManagerTester_la_LIBADD = -lkunittest ../libkwordpart.la
kunittest_KWPageManagerTester_la_LDFLAGS = -module $(KDE_CHECK_PLUGIN) $(all_libraries)

check-local: kunittest_KWPageManagerTester.la
	kunittestmodrunner

