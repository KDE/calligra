include $(top_srcdir)/kexi/Makefile.global

noinst_PROGRAMS = kexidbdrivercombotest

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)

kexidbdrivercombotest_SOURCES = kexidbdrivercombotest.cpp
kexidbdrivercombotest_LDADD = $(LIB_QT) $(LIB_KDECORE) \
 $(top_builddir)/kexi/kexidb/libkexidb.la \
 $(top_builddir)/kexi/widget/libkexiextendedwidgets.la
kexidbdrivercombotest_LDFLAGS = $(all_libraries) $(KDE_RPATH)

METASOURCES = AUTO

