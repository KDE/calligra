include $(top_srcdir)/kexi/Makefile.global

bin_PROGRAMS = kexitableviewtest  #kexidbfirebirdcursor

noinst_PROGRAMS = kexitableviewtest

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)

SUBDIRS = .

METASOURCES = AUTO

kexitableviewtest_SOURCES = main.cpp
kexitableviewtest_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la $(top_builddir)/kexi/widget/libkexiextendedwidgets.la $(all_libraries)
kexitableviewtest_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(VER_INFO) --no-undefined

