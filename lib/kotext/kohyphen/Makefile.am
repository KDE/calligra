INCLUDES = $(all_includes)
KDE_CXXFLAGS = -fexceptions

METASOURCES = AUTO

noinst_HEADERS = hnjalloc.h hyphen.h kohyphen.h

noinst_LTLIBRARIES = libkohyphen.la
libkohyphen_la_SOURCES = hnjalloc.c hyphen.c kohyphen.cpp

dicts_DATA = dicts.xml hyph_cs.dic hyph_en.dic hyph_ru.dic
dictsdir = $(kde_datadir)/koffice/hyphdicts

EXTRA_DIST = $(dicts_DATA)

check_PROGRAMS = kohyphentest

kohyphentest_SOURCES = kohyphentest.cpp
kohyphentest_LDADD = ./libkohyphen.la $(LIB_QT) $(LIB_KDECORE)
kohyphentest_LDFLAGS = $(KDE_RPATH) $(all_libraries)
