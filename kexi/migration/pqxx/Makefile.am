include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = keximigrate_pqxx.la

INCLUDES = -I$(srcdir)/../../.. -I$(top_srcdir)/kexi $(all_includes) -I$(PG_INCDIR) -I$(PQXX_INCDIR)

keximigrate_pqxx_la_METASOURCES = AUTO

keximigrate_pqxx_la_SOURCES = pqxxmigrate.cpp

#TODO share -libs with pqxx kexidb drv!
#keximigrate_pqxx_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lcom_err -lkrb5 -lssl -lcrypto -lcrypt -lpqxx ../libkeximigrate.la
keximigrate_pqxx_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lpqxx ../libkeximigrate.la

keximigrate_pqxx_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) -L$(PQXX_LIBDIR) -L$(PG_LIBDIR) $(VER_INFO) -no-undefined

kde_services_DATA = keximigrate_pqxx.desktop

noinst_HEADERS =  pqxxmigrate.h pg_type.h

