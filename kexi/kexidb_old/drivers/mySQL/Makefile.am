kde_module_LTLIBRARIES = keximysqlinterface.la

INCLUDES = -I$(srcdir)/../.. $(all_includes)

LDFLAGS = $(all_libraries) $(KDE_RPATH) $(MYSQL_LDFLAGS)

keximysqlinterface_la_METASOURCES = AUTO

keximysqlinterface_la_SOURCES = mysqldb.cpp mysqlresult.cpp mysqlrecord.cpp

keximysqlinterface_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lmysqlclient ../../libkexidb.la

keximysqlinterface_la_LDFLAGS     = -module $(KDE_PLUGIN) -L$(MYSQL_LIBDIR)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

kde_services_DATA = mysqlinterface.desktop

