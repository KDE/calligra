include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexidb_sqlite2driver.la

INCLUDES = -I$(top_srcdir)/kexi \
	-I$(top_srcdir)/kexi/kexidb \
	-I$(top_srcdir)/kexi/3rdparty/kexisql/src \
	-I$(srcdir)/../sqlite \
	-I$(srcdir)/../sqlite/moc \
	-I../sqlite \
	$(all_includes)

kexidb_sqlite2driver_la_METASOURCES = AUTO

kexidb_sqlite2driver_la_SOURCES = sqliteconnection.cpp sqlitedriver.cpp sqlitecursor.cpp \
	sqlitepreparedstatement.cpp sqliteadmin.cpp

kexidb_sqlite2driver_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) \
 $(top_builddir)/kexi/3rdparty/kexisql/src/libkexisql2.la \
 $(top_builddir)/kexi/kexidb/libkexidb.la \
 $(top_builddir)/kexi/kexidb/parser/libkexidbparser.la

kexidb_sqlite2driver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO) 


kde_services_DATA = kexidb_sqlite2driver.desktop


KDE_CXXFLAGS += -DKEXIDB_SQLITE_DRIVER_EXPORT= -D__KEXIDB__= \
	-DSQLITE2= -include $(top_srcdir)/kexi/kexidb/global.h

