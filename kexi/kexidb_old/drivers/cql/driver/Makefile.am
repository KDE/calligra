kde_module_LTLIBRARIES = kexicqlinterface.la

INCLUDES = -I$(srcdir)/../../../ $(all_includes)

LDFLAGS = $(all_libraries) $(KDE_RPATH)

keximysqlinterface_la_METASOURCES = AUTO

keximysqlinterface_la_SOURCES = cqldb.cpp 

keximysqlinterface_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lmysqlclient ../../libkexidb.la

keximysqlinterface_la_LDFLAGS     = -module $(KDE_PLUGIN) -L$(MYSQL_LIBDIR)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

install-data-local:
	$(mkinstalldirs) $(kde_servicedir)
	$(INSTALL_DATA) $(srcdir)/mysqlinterface.desktop $(kde_servicesdir)/mysqlinterface.desktop

