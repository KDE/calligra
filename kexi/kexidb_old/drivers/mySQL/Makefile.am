kde_module_LTLIBRARIES = keximysqlinterface.la

INCLUDES = $(all_includes)

LDFLAGS = $(all_libraries) $(KDE_RPATH) $(MYSQL_LDFLAGS)

keximysqlinterface_la_METASOURCES = AUTO

keximysqlinterface_la_SOURCES = mysqldb.cpp mysqlfield.cpp mysqlresult.cpp mysqlrecord.cpp 

keximysqlinterface_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) $(LIB_MYSQL) ../../kexiDB/libkexidb.la

keximysqlinterface_la_LDFLAGS     = -module $(KDE_PLUGIN) $(MYSQL_LDFLAGS)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

install-data-local:
	$(mkinstalldirs) $(kde_servicedir)
	$(INSTALL_DATA) $(srcdir)/mysqlinterface.desktop $(kde_servicesdir)/mysqlinterface.desktop

