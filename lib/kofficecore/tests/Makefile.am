####### General stuff

KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES= -I$(srcdir)/.. $(all_includes)

####### Files

check_PROGRAMS = priorityqueue_test filterchain_test

priorityqueue_test_SOURCES = priorityqueue_test.cpp
priorityqueue_test_LDADD = ../libkofficecore.la

filterchain_test_SOURCES = filterchain_test.cpp
filterchain_test_LDADD = ../libkofficecore.la
