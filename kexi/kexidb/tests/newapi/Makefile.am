bin_PROGRAMS = kexidbtest

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)

SUBDIRS = .

kexidbtest_METASOURCES = AUTO

kexidbtest_SOURCES = main.cpp

kexidbtest_LDADD = $(LIB_QT) $(LIB_KDECORE) ../libkexidb.la  $(all_libraries)
kexidbtest_LDFLAGS = $(all_libraries) $(KDE_RPATH) --no-undefined

include $(top_srcdir)/kexi/Makefile.global
