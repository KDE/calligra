lib_LTLIBRARIES = libmysqlinterface.la


INCLUDES = $(all_includes)

LDFLAGS =

libmysqlinterface_la_METASOURCES = AUTO

libmysqlinterface_la_SOURCES = mysqldb.cpp 

install-data-local:
	$(mkinstalldirs) $(kde_servicedir)
	$(INSTALL_DATA) $(srcdir)/mysqlinterface.desktop $(kde_servicesdir)/mysqlinterface.desktop

