




########### install files ###############






#original Makefile.am contents follow:

#AM_CPPFLAGS = -I$(srcdir)/../ $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) $(all_includes)
#
#INCLUDES = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
#
## The check_ target makes sure we don't install the modules,
## $(KDE_CHECK_PLUGIN) assures a shared library is created.
#check_LTLIBRARIES = kunittest_KWPageManagerTester.la \
#		kunittest_FrameTester.la \
#		kunittest_FrameViewTester.la
#
#kunittest_KWPageManagerTester_la_SOURCES = KWPageManagerTester.cpp
#kunittest_KWPageManagerTester_la_LIBADD = -lkunittest ../libkwordprivate.la
#kunittest_KWPageManagerTester_la_LDFLAGS = -module $(KDE_CHECK_PLUGIN) $(all_libraries)
#
#kunittest_FrameTester_la_SOURCES = FrameTester.cpp
#kunittest_FrameTester_la_LIBADD = -lkunittest ../libkwordprivate.la
#kunittest_FrameTester_la_LDFLAGS = -module $(KDE_CHECK_PLUGIN) $(all_libraries)
#
#kunittest_FrameViewTester_la_SOURCES = FrameViewTester.cpp
#kunittest_FrameViewTester_la_LIBADD = -lkunittest ../libkwordprivate.la
#kunittest_FrameViewTester_la_LDFLAGS = -module $(KDE_CHECK_PLUGIN) $(all_libraries)
#
#check-local: kunittest_KWPageManagerTester.la kunittest_FrameTester.la kunittest_FrameViewTester.la
#	kunittestmodrunner 2>&1 | perl $(srcdir)/checkFilter.pl
#
