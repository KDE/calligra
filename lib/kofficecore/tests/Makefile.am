####### General stuff

KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES= -I$(srcdir)/.. $(KSTORE_INCLUDES) $(all_includes)

####### Files

check_PROGRAMS = priorityqueue_test filterchain_test filter_graph kogenstylestest \
	kodomtest

TESTS = kogenstylestest kodomtest

priorityqueue_test_SOURCES = priorityqueue_test.cpp
priorityqueue_test_LDADD = ../libkofficecore.la

filterchain_test_SOURCES = filterchain_test.cpp
filterchain_test_LDADD = ../libkofficecore.la

filter_graph_SOURCES = filter_graph.cpp
filter_graph_LDADD = ../libkofficecore.la

kogenstylestest_SOURCES = kogenstylestest.cpp
kogenstylestest_LDADD = ../libkofficecore.la

kodomtest_SOURCES = kodomtest.cpp
kodomtest_LDADD = ../libkofficecore.la

dot:
	dot -Tpng -o graph.png graph.dot
