lib_LTLIBRARIES = libkexidb.la

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)

SUBDIRS = . drivers tests

libkexidb_la_METASOURCES = AUTO

libkexidb_la_SOURCES = drivermanager.cpp object.cpp driver.cpp connection.cpp connectiondata.cpp table.cpp \
	cursor.cpp transaction.cpp field.cpp fieldlist.cpp index.cpp query.cpp

libkexidb_la_LIBADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KIO)
libkexidb_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined

kde_servicetypes_DATA = kexidb_driver.desktop

include $(top_srcdir)/kexi/Makefile.global
