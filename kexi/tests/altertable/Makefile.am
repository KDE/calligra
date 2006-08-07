include $(top_srcdir)/kexi/Makefile.global

noinst_PROGRAMS = kexialtertabletest
 
INCLUDES = -I$(top_srcdir)/kexi \
  -I$(top_srcdir)/kexi/widget \
  -I$(top_srcdir)/kexi/core \
  $(all_includes)

SUBDIRS = .

METASOURCES = AUTO

kexidbtest_SOURCES = altertable.cpp
kexidbtest_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la \
 ../../kexidb/parser/libkexidbparser.la \
 $(top_builddir)/kexi/widget/libkexiextendedwidgets.la
kexidbtest_LDFLAGS = $(all_libraries) $(KDE_RPATH)

