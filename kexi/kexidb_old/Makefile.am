lib_LTLIBRARIES = libkexidb.la

INCLUDES = $(all_includes)

SUBDIRS = .

LDFLAGS = $(all_libraries) $(KDE_RPATH)

libkexidb_la_METASOURCES = AUTO

libkexidb_la_SOURCES = kexidbdriver.cpp kexidbinterfacemanager.cpp kexidb.cpp

libkexidb_la_LIBADD = $(LIB_KPARTS)

install-data-local:
	$(mkinstalldirs) $(kde_servicetypesdir)/
	$(INSTALL_DATA) $(srcdir)/kexidbdriver.desktop $(kde_servicetypesdir)/kexidbdriver.desktop

