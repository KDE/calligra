lib_LTLIBRARIES = libkexidb.la

INCLUDES = $(all_includes)

SUBDIRS = . drivers

LDFLAGS = $(all_libraries) $(KDE_RPATH)

libkexidb_la_METASOURCES = AUTO

libkexidb_la_SOURCES = kexidbdriver.cpp kexidbinterfacemanager.cpp kexidbfield.cpp kexidbresult.cpp kexidbrecord.cpp kexidb.cpp kexidberror.cpp

libkexidb_la_LIBADD = $(LIB_KPARTS)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

install-data-local:
	$(mkinstalldirs) $(kde_servicetypesdir)/
	$(INSTALL_DATA) $(srcdir)/kexidbdriver.desktop $(kde_servicetypesdir)/kexidbdriver.desktop

