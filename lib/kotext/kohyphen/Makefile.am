INCLUDES = $(all_includes)
KDE_CXXFLAGS = -fexceptions

METASOURCES = AUTO

noinst_HEADERS = hnjalloc.h hyphen.h kohyphen.h

noinst_LTLIBRARIES = libkohyphen.la
libkohyphen_la_SOURCES = hnjalloc.c hyphen.c kohyphen.cpp

dictsdir = $(kde_datadir)/koffice/hyphdicts

SUBDIRS = . hyphdicts

check_PROGRAMS = kohyphentest

kohyphentest_SOURCES = kohyphentest.cpp
kohyphentest_LDADD = ./libkohyphen.la $(LIB_QT) $(LIB_KDECORE)
kohyphentest_LDFLAGS = $(KDE_RPATH) $(all_libraries)
