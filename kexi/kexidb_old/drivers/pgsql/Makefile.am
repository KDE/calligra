kde_module_LTLIBRARIES = kexipgsqlinterface.la

INCLUDES = -I$(top_srcdir)/kexi/kexiDB $(all_includes)

kexipgsqlinterface_la_METASOURCES = AUTO

kexipgsqlinterface_la_SOURCES = pgsqldb.cpp

kexipgsqlinterface_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lpq $(top_builddir)/kexi/kexiDB/libkexidb.la

kexipgsqlinterface_la_LDFLAGS = $(all_libraries) -L$(PG_LIBDIR) -module $(KDE_PLUGIN)

noinst_HEADERS = pgsqldb.hpp

kde_services_DATA = kexipgsqlinterface.desktop

include $(top_srcdir)/kexi/Makefile.global
