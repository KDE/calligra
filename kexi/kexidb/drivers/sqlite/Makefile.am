kde_module_LTLIBRARIES = kexidb_sqlitedriver.la

INCLUDES = -I$(srcdir)/../.. -I$(top_srcdir)/kexi $(all_includes)

kexidb_sqlitedriver_la_METASOURCES = AUTO

kexidb_sqlitedriver_la_SOURCES = sqliteconnection.cpp sqlitedriver.cpp

kexidb_sqlitedriver_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lsqlite ../../libkexi_db.la 

kexidb_sqlitedriver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) -L/usr/local/lib


kde_services_DATA = kexidb_sqlitedriver.desktop

include $(top_srcdir)/kexi/Makefile.global
