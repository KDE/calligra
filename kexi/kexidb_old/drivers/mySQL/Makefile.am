kde_module_LTLIBRARIES = libmysqlinterface.la


INCLUDES = $(all_includes)

LDFLAGS = $(all_libraries) $(KDE_RPATH)

libmysqlinterface_la_METASOURCES = AUTO

libmysqlinterface_la_SOURCES = mysqldb.cpp 

libmysqlinterface_la_LIBADD = $(LIB_KPARTS)

install-data-local:
	$(mkinstalldirs) $(kde_servicedir)
	$(INSTALL_DATA) $(srcdir)/mysqlinterface.desktop $(kde_servicesdir)/mysqlinterface.desktop

