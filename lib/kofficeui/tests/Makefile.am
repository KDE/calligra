####### General stuff

KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES= -I$(srcdir)/.. $(KOFFICECORE_INCLUDES) $(all_includes)

####### Files

METASOURCES = AUTO

check_PROGRAMS = coloraction_test korecttest

coloraction_test_SOURCES = coloraction_test.cpp
coloraction_test_LDADD = ../libkofficeui.la

korecttest_SOURCES = korecttest.cpp
korecttest_LDADD = ../libkofficeui.la
