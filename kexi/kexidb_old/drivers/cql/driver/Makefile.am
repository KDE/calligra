kde_module_LTLIBRARIES = libkexicqlinterface.la

INCLUDES = -I$(srcdir)/../../../ -I$(srcdir)/../cql/sql -I$(srcdir)/../cql/common $(all_includes)

LDFLAGS = $(all_libraries) $(KDE_RPATH)

libkexicqlinterface_la_METASOURCES = AUTO

libkexicqlinterface_la_SOURCES = cqldb.cpp 

libkexicqlinterface_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) ../../../libkexidb.la ../cql/sql/libcql.la

libkexicqlinterface_la_LDFLAGS     = -module $(KDE_PLUGIN)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

install-data-local:
	$(mkinstalldirs) $(kde_servicedir)
	$(INSTALL_DATA) $(srcdir)/cqlinterface.desktop $(kde_servicesdir)/cqlinterface.desktop

