include $(top_srcdir)/kexi/Makefile.global

bin_PROGRAMS = keximigratetest

noinst_PROGRAMS = keximigratetest 

noinst_HEADERS = keximigrate.h pqxxmigrate.h pg_type.h txtmigrate.h \
	migratecontrol.h importwizard.h 

INCLUDES = -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi $(all_includes) \
	-I$(PG_INCDIR) -I$(PQXX_INCDIR)

SUBDIRS = .

METASOURCES = AUTO

keximigratetest_SOURCES = keximigratetest.cpp keximigrate.cpp pqxxmigrate.cpp migratecontrol.cpp importwizard.cpp
keximigratetest_LDADD = $(top_builddir)/kexi/main/libkeximain.la \
	$(top_builddir)/kexi/core/libkexicore.la $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la \
	-lpqxx
keximigratetest_LDFLAGS = -no-undefined $(all_libraries) $(KDE_RPATH) \
	-L$(PQXX_LIBDIR) -L$(PG_LIBDIR)

