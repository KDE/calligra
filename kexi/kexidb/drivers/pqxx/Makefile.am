kde_module_LTLIBRARIES = kexidb_pqxxsqldriver.la

INCLUDES = -I$(srcdir)/../../.. $(all_includes) -I/usr/local/pqxx/include -I/usr/local/pgsql/include

kexidb_pqxxsqldriver_la_METASOURCES = AUTO

kexidb_pqxxsqldriver_la_SOURCES = pqxxdriver.cpp pqxxcursor.cpp pqxxconnection.cpp

kexidb_pqxxsqldriver_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lpqxx ../../libkexidb.la

kexidb_pqxxsqldriver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) -L/usr/local/pqxx/lib -no-undefined

kde_services_DATA = kexidb_pqxxsqldriver.desktop

include $(top_srcdir)/kexi/Makefile.global
noinst_HEADERS =  pqxxconnection.h
