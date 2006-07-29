include $(top_srcdir)/kexi/Makefile.global

# unused: kexidbmysqlcursor kexidbfirebirdcursor

noinst_PROGRAMS = kexidbtest
 
# unused: kexidbmysqlcursor

INCLUDES = -I$(top_srcdir)/kexi \
  -I$(top_srcdir)/kexi/widget \
  -I$(top_srcdir)/kexi/core \
  $(all_includes)

SUBDIRS = .

METASOURCES = AUTO

kexidbtest_SOURCES = main.cpp
kexidbtest_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la \
 ../../kexidb/parser/libkexidbparser.la \
 $(top_builddir)/kexi/widget/libkexiextendedwidgets.la
kexidbtest_LDFLAGS = $(all_libraries) $(KDE_RPATH)

#kexidbmysqlcursor_SOURCES = mysqlcursor.cpp
#kexidbmysqlcursor_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la \
# ../../kexidb/parser/libkexidbparser.la
#kexidbmysqlcursor_LDFLAGS = $(all_libraries) $(KDE_RPATH)

#kexidbfirebirdcursor_SOURCES = firebirdcursor.cpp
#kexidbfirebirdcursor_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la
#kexidbfirebirdcursor_LDFLAGS = $(all_libraries) $(KDE_RPATH)

