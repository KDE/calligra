kde_module_LTLIBRARIES = keximysqlinterface.la

INCLUDES = -I$(srcdir)/../.. $(all_includes)

keximysqlinterface_la_METASOURCES = AUTO

keximysqlinterface_la_SOURCES = mysqldb.cpp mysqlresult.cpp mysqlrecord.cpp

keximysqlinterface_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lmysqlclient ../../libkexidb.la

keximysqlinterface_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) -L$(MYSQL_LIBDIR)

KDE_CXXFLAGS = $(USE_EXCEPTIONS) -DKEXIDB_EXPORT= -DKDECORE_EXPORT= -DKEXI_MYSQL_IFACE_EXPORT=

kde_services_DATA = keximysqlinterface.desktop

