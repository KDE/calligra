kde_module_LTLIBRARIES = kexiqtsqlinterface.la

INCLUDES = -I$(srcdir)/../.. $(all_includes)

kexiqtsqlinterface_la_METASOURCES = AUTO

kexiqtsqlinterface_la_SOURCES = qtsqldb.cpp qtsqlrecord.cpp

kexiqtsqlinterface_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lmysqlclient ../../libkexidb.la

kexiqtsqlinterface_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) -L$(MYSQL_LIBDIR)

#KDE_CXXFLAGS = $(USE_EXCEPTIONS)

kde_services_DATA = kexiqtsqlinterface.desktop

include $(top_srcdir)/kexi/Makefile.global
