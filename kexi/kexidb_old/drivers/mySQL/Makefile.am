kde_module_LTLIBRARIES = keximysqlinterface.la

INCLUDES = $(all_includes)

LDFLAGS = $(all_libraries) $(KDE_RPATH)

keximysqlinterface_la_METASOURCES = AUTO

keximysqlinterface_la_SOURCES = mysqldb.cpp 

keximysqlinterface_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) ../../kexiDB/libkexidb.la

keximysqlinterface_la_LDFLAGS     = -module $(KDE_PLUGIN)

install-data-local:
	$(mkinstalldirs) $(kde_servicedir)
	$(INSTALL_DATA) $(srcdir)/mysqlinterface.desktop $(kde_servicesdir)/mysqlinterface.desktop

