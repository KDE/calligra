INCLUDES = $(KOTEXT_INCLUDES) $(all_includes)
KDE_CXXFLAGS = $(USE_EXCEPTIONS)

METASOURCES = AUTO

check_PROGRAMS = kotextformattertest

kotextformattertest_SOURCES = kotextformattertest.cpp
kotextformattertest_LDADD = ../libkotext.la
kotextformattertest_LDFLAGS = $(KDE_RPATH) $(all_libraries)
