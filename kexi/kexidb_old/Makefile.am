lib_LTLIBRARIES = libkexidb.la

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)

SUBDIRS = . drivers

libkexidb_la_METASOURCES = AUTO

libkexidb_la_SOURCES = kexidbinterfacemanager.cpp kexidbfield.cpp kexidbrecordset.cpp kexidb.cpp kexidberror.cpp \
	kexidbwatcher.cpp kexidbtable.cpp kexidbupdaterecord.cpp

libkexidb_la_LIBADD = $(LIB_KPARTS)
libkexidb_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) --no-undefined

kde_servicetypes_DATA = kexidbdriver.desktop

include $(top_srcdir)/kexi/Makefile.global
