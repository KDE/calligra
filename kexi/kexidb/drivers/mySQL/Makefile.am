include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexidb_mysqldriver.la

INCLUDES = -I$(srcdir)/../../.. $(all_includes) \
  -I$(MYSQL_INC) \
  -I$(srcdir)/../..  \
  -I$(top_srcdir)/kexi

kexidb_mysqldriver_la_METASOURCES = AUTO

kexidb_mysqldriver_la_SOURCES = \
  mysqldriver.cpp \
  mysqlconnection.cpp \
  mysqlconnection_p.cpp \
  mysqlcursor.cpp \
  mysqlkeywords.cpp

kexidb_mysqldriver_la_LIBADD = $(LIB_KPARTS) \
  $(LIB_QT) \
  $(MYSQL_LIBS) \
  -lmysqlclient \
  ../../libkexidb.la

kexidb_mysqldriver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO) -no-undefined


kde_services_DATA = kexidb_mysqldriver.desktop


KDE_CXXFLAGS += -DKEXIDB_MYSQL_DRIVER_EXPORT= -D__KEXIDB__= \
	-include $(top_srcdir)/kexi/kexidb/global.h

