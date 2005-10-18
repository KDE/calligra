include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexidb_sqlite3driver.la

INCLUDES = -I$(top_srcdir)/kexi/3rdparty/kexisql3/src -I$(srcdir)/../..  \
	-I$(top_srcdir)/kexi $(all_includes)

kexidb_sqlite3driver_la_METASOURCES = AUTO

kexidb_sqlite3driver_la_SOURCES = sqliteconnection.cpp sqlitedriver.cpp sqlitecursor.cpp \
sqlitekeywords.cpp sqlitepreparedstatement.cpp

kexidb_sqlite3driver_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) \
 $(top_builddir)/kexi/3rdparty/kexisql3/src/libkexisql3.la \
 $(top_builddir)/kexi/kexidb/libkexidb.la \
 $(top_builddir)/kexi/kexidb/parser/libkexidbparser.la

kexidb_sqlite3driver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO) 


kde_services_DATA = kexidb_sqlite3driver.desktop


KDE_CXXFLAGS += -DKEXIDB_SQLITE_DRIVER_EXPORT= -D__KEXIDB__= \
	-include $(top_srcdir)/kexi/kexidb/global.h

