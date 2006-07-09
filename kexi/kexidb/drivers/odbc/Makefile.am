include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexidb_odbcdriver.la

INCLUDES = -I$(srcdir)/../..  -I$(top_srcdir)/kexi $(all_includes)

kexidb_odbcdriver_la_METASOURCES = AUTO

kexidb_odbcdriver_la_SOURCES = odbcdriver.cpp odbcconnection.cpp 

kexidb_odbcdriver_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lodbc $(top_builddir)/kexi/kexidb/libkexidb.la

kexidb_odbcdriver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO) -no-undefined


kde_services_DATA = kexidb_odbcdriver.desktop


KDE_CXXFLAGS += -DKEXIDB_ODBC_DRIVER_EXPORT= -D__KEXIDB__= \
	-include $(top_srcdir)/kexi/kexidb/global.h

