include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = keximigrate_txtmigrate.la

INCLUDES = -I$(srcdir)/../../.. -I$(top_srcdir)/kexi $(all_includes)

keximigrate_txtmigrate_la_METASOURCES = AUTO

keximigrate_txtmigrate_la_SOURCES = txtmigrate.cpp

keximigrate_txtmigrate_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) ../libkeximigrate.la

keximigrate_txtmigrate_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO) -no-undefined

kde_services_DATA = keximigrate_txtmigrate.desktop

noinst_HEADERS =  txtmigrate.h

