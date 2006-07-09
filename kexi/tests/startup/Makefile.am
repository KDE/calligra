include $(top_srcdir)/kexi/Makefile.global

bin_PROGRAMS = kexistartuptest

noinst_PROGRAMS = kexistartuptest

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)

SUBDIRS = .

METASOURCES = AUTO

kexistartuptest_SOURCES = main.cpp
kexistartuptest_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la \
 $(top_builddir)/kexi/core/libkexicore.la \
 $(all_libraries)

kexistartuptest_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(VER_INFO) --no-undefined

