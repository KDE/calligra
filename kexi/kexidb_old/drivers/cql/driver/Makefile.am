kde_module_LTLIBRARIES = kexicqlinterface.la

INCLUDES = -I$(srcdir)/../../../ -I$(srcdir)/../cql/sql -I$(srcdir)/../cql/common $(all_includes)

LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined

kexicqlinterface_la_METASOURCES = AUTO

kexicqlinterface_la_SOURCES = cqldb.cpp cqlrecord.cpp cqlfield.cpp 

kexicqlinterface_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) ../../../libkexidb.la ../cql/sql/libcql.la

kexicqlinterface_la_LDFLAGS     = -module -no-undefined $(KDE_PLUGIN)

bin_PROGRAMS = kexitest
kexitest_SOURCES = ctest.cpp
kexitest_LDADD = ../cql/sql/libcql.la

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

install-data-local:
	$(mkinstalldirs) $(kde_servicedir)
	$(INSTALL_DATA) $(srcdir)/cqlinterface.desktop $(kde_servicesdir)/cqlinterface.desktop

