include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = keximigrate_mysql.la

INCLUDES = -I$(srcdir)/../../.. -I$(top_srcdir)/kexi $(all_includes) -I$(MYSQL_INC)

keximigrate_mysql_la_METASOURCES = AUTO

keximigrate_mysql_la_SOURCES = mysqlmigrate.cpp

keximigrate_mysql_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) ../libkeximigrate.la $(MYSQL_LIBS) -lmysqlclient

keximigrate_mysql_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO) -no-undefined

kde_services_DATA = keximigrate_mysql.desktop

noinst_HEADERS =  mysqlmigrate.h

