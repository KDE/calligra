lib_LTLIBRARIES = libkexidb.la

INCLUDES = $(all_includes)

SUBDIRS = . drivers

LDFLAGS = $(all_libraries) $(KDE_RPATH)

libkexidb_la_METASOURCES = AUTO

libkexidb_la_SOURCES = kexidbinterfacemanager.cpp kexidbfield.cpp kexidbresult.cpp kexidbrecord.cpp kexidb.cpp kexidberror.cpp kexidbwatcher.cpp kexidbtable.cpp

libkexidb_la_LIBADD = $(LIB_KPARTS)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

kde_servicetypes_DATA = kexidbdriver.desktop

