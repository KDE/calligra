kde_module_LTLIBRARIES = keximysqlinterface.la

INCLUDES = -I$(srcdir)/../.. $(all_includes) -I$(MYSQL_INC)

keximysqlinterface_la_METASOURCES = AUTO

keximysqlinterface_la_SOURCES = mysqldb.cpp mysqlresult.cpp mysqlrecord.cpp

keximysqlinterface_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) $(MYSQL_LIBS) -lmysqlclient ../../libkexidb.la

keximysqlinterface_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) -L$(MYSQL_LIBDIR)


kde_services_DATA = keximysqlinterface.desktop

include $(top_srcdir)/kexi/Makefile.global
