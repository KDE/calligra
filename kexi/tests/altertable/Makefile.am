include $(top_srcdir)/kexi/Makefile.global

noinst_PROGRAMS = kexialtertabletest

INCLUDES = -I$(top_srcdir)/kexi \
  -I$(top_srcdir)/kexi/widget \
  -I$(top_srcdir)/kexi/core \
  -I$(top_srcdir)/lib \
  $(all_includes)

SUBDIRS = .

METASOURCES = AUTO

kexialtertabletest_SOURCES = altertable.cpp
kexialtertabletest_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la \
 $(top_builddir)/kexi/kexiutils/libkexiutils.la \
 $(top_builddir)/kexi/main/libkeximain.la \
 $(top_builddir)/kexi/kexidb/parser/libkexidbparser.la \
 $(top_builddir)/kexi/widget/libkexiextendedwidgets.la
kexialtertabletest_LDFLAGS = $(all_libraries) $(KDE_RPATH)

