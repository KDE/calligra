kde_module_LTLIBRARIES = kexipqxxsqlinterface.la

INCLUDES = -I$(srcdir)/../.. $(all_includes) -I/usr/local/pgsql/include/ -I/usr/local/pqxx/include/

kexipqxxsqlinterface_la_METASOURCES = AUTO

kexipqxxsqlinterface_la_SOURCES = pqxxsqldb.cpp pqxxsqlrecord.cpp pqxxsqlresult.cpp

kexipqxxsqlinterface_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) ../../libkexidb.la -lpqxx

kexipqxxsqlinterface_la_LDFLAGS = -module  $(KDE_PLUGIN) -L/usr/local/pqxx/lib $(all_libraries)

KDE_CXXFLAGS = $(USE_EXCEPTIONS) -DKEXIDB_EXPORT= -DKDECORE_EXPORT= -DKEXI_MYSQL_IFACE_EXPORT=

kde_services_DATA = kexipqxxsqlinterface.desktop

