include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkeximigrate.la

INCLUDES = -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi $(all_includes) -I$(PG_INCDIR) -I$(PQXX_INCDIR)

SUBDIRS = . pqxx

libkeximigrate_la_METASOURCES = AUTO

libkeximigrate_la_SOURCES = keximigrate.cpp importwizard.cpp migratemanager.cpp

libkeximigrate_la_LIBADD = \
	$(top_builddir)/kexi/core/libkexicore.la $(LIB_QT) $(LIB_KDECORE) \
	$(top_builddir)/kexi/kexidb/libkexidb.la

libkeximigrate_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) \
-L$(PQXX_LIBDIR) -L$(PG_LIBDIR)


noinst_HEADERS = keximigrate.h migratemanager.h importwizard.h migratemanager_p.h

KDE_OPTIONS=nofinal
