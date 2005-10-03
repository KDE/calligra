INCLUDES = $(KOTEXT_INCLUDES) $(all_includes)
KDE_CXXFLAGS = $(USE_EXCEPTIONS)

METASOURCES = AUTO

check_PROGRAMS = kotextformattertest kobordertest kovariabletest

LDADD = ../libkotext.la
AM_LDFLAGS = $(KDE_RPATH) $(all_libraries)

kobordertest_SOURCES = kobordertest.cpp
kotextformattertest_SOURCES = kotextformattertest.cpp
kovariabletest_SOURCES = kovariabletest.cpp
