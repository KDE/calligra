include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexidb_sqlitedriver.la

INCLUDES = -I$(top_srcdir)/kexi/3rdparty/kexisql/src -I$(srcdir)/../..  -I$(top_srcdir)/kexi $(all_includes)

kexidb_sqlitedriver_la_METASOURCES = AUTO

kexidb_sqlitedriver_la_SOURCES = sqliteconnection.cpp sqlitedriver.cpp sqlitecursor.cpp

kexidb_sqlitedriver_la_LIBADD = $(LIB_KPARTS) $(LIB_QT)  $(top_builddir)/kexi/3rdparty/kexisql/src/libkexisql.la $(top_builddir)/kexi/kexidb/libkexidb.la

kexidb_sqlitedriver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) 


kde_services_DATA = kexidb_sqlitedriver.desktop


KDE_CXXFLAGS += -DKEXIDB_SQLITE_DRIVER_EXPORT= -D__KEXIDB__= \
	-include $(top_srcdir)/kexi/kexidb/global.h

