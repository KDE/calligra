lib_LTLIBRARIES = libkexidb.la

INCLUDES = $(all_includes)

SUBDIRS = . drivers

libkexidb_la_METASOURCES = AUTO

libkexidb_la_SOURCES = kexidbinterfacemanager.cpp kexidbfield.cpp kexidbrecordset.cpp kexidb.cpp kexidberror.cpp \
	kexidbwatcher.cpp kexidbtable.cpp kexidbupdaterecord.cpp

libkexidb_la_LIBADD = $(LIB_KPARTS)
libkexidb_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) --no-undefined

KDE_CXXFLAGS = $(USE_EXCEPTIONS) -DKEXIDB_EXPORT=

kde_servicetypes_DATA = kexidbdriver.desktop

