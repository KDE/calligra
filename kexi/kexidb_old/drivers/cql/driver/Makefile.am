kde_module_LTLIBRARIES = kexicqlinterface.la

INCLUDES = -I$(srcdir)/../../../ -I$(srcdir)/../cql/sql -I$(srcdir)/../cql/common $(all_includes)

kexicqlinterface_la_METASOURCES = AUTO

kexicqlinterface_la_SOURCES = cqldb.cpp cqlrecord.cpp 
kexicqlinterface_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) ../../../libkexidb.la ../cql/sql/libcql.la
kexicqlinterface_la_LDFLAGS     = -module -no-undefined $(KDE_PLUGIN)

bin_PROGRAMS = kexitest
kexitest_SOURCES = ctest.cpp
kexitest_LDADD = $(LIB_KPARTS) $(LIB_QT) ../cql/sql/libcql.la kexicqlinterface.la
kexitest_LDFLAGS = $(all_libraries)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

kde_services_DATA = kexicqlinterface.desktop

