lib_LTLIBRARIES = libkexidb.la

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)

SUBDIRS = . parser drivers tests

libkexidb_la_METASOURCES = AUTO

libkexidb_la_SOURCES = drivermanager.cpp object.cpp driver.cpp connection.cpp \
 connectiondata.cpp tableschema.cpp cursor.cpp transaction.cpp field.cpp fieldlist.cpp \
 indexschema.cpp queryschema.cpp schemadata.cpp global.cpp

libkexidb_la_LIBADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KIO)
libkexidb_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined

kde_servicetypes_DATA = kexidb_driver.desktop

include $(top_srcdir)/kexi/Makefile.global

KDE_CXXFLAGS += -D__KEXIDB__= -include ./global.h

