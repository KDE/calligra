noinst_PROGRAMS = krosstest

krosstest_SOURCES = test.cpp

krosstest_LDADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    ../api/libkrossapi.la \
    ../kexidb/libkrosskexidb.la \
    ../main/libkrossmain.la

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)
KDE_CXXFLAGS = $(USE_EXCEPTIONS)
krosstest_LDFLAGS = $(all_libraries) $(KDE_RPATH)
SUBDIRS = .
METASOURCES = AUTO
