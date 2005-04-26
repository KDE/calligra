####### General stuff

KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES= -I$(srcdir)/.. $(KOFFICECORE_INCLUDES) $(all_includes)

####### Files

METASOURCES = AUTO

check_PROGRAMS = kopointtest korecttest # coloraction_test

TESTS = kopointtest korecttest

#coloraction_test_SOURCES = coloraction_test.cpp
#coloraction_test_LDADD = ../libkofficeui.la

kopointtest_SOURCES = kopointtest.cpp
kopointtest_LDADD = ../libkofficeui.la

korecttest_SOURCES = korecttest.cpp
korecttest_LDADD = ../libkofficeui.la
