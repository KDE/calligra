kde_module_LTLIBRARIES = kexidb_mysqldriver.la

INCLUDES = -I$(srcdir)/../../.. $(all_includes) -I$(MYSQL_INC)

kexidb_mysqldriver_la_METASOURCES = AUTO

kexidb_mysqldriver_la_SOURCES = mysqldriver.cpp mysqlconnection.cpp mysqlcursor.cpp

kexidb_mysqldriver_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) $(MYSQL_LIBS) -lmysqlclient ../../libkexidb.la

kexidb_mysqldriver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) -L$(MYSQL_LIBDIR) -no-undefined


kde_services_DATA = kexidb_mysqldriver.desktop

include $(top_srcdir)/kexi/Makefile.global
