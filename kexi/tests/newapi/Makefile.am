bin_PROGRAMS = kexidbtest kexidbmysqlcursor #kexidbfirebirdcursor

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)

SUBDIRS = .

METASOURCES = AUTO

kexidbtest_SOURCES = main.cpp
kexidbtest_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_srcdir)/kexi/kexidb/libkexidb.la $(top_srcdir)/kexi/tableview/libkexidatatable.la $(all_libraries)
kexidbtest_LDFLAGS = $(all_libraries) $(KDE_RPATH) --no-undefined

kexidbmysqlcursor_SOURCES = mysqlcursor.cpp
kexidbmysqlcursor_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_srcdir)/kexi/kexidb/libkexidb.la  $(all_libraries)
kexidbmysqlcursor_LDFLAGS = $(all_libraries) $(KDE_RPATH) --no-undefined

#kexidbfirebirdcursor_SOURCES = firebirdcursor.cpp
#kexidbfirebirdcursor_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_srcdir)/kexi/kexidb/libkexidb.la  $(all_libraries)
#kexidbfirebirdcursor_LDFLAGS = $(all_libraries) $(KDE_RPATH) --no-undefined

include $(top_srcdir)/kexi/Makefile.global
