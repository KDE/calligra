include $(top_srcdir)/kexi/Makefile.global

bin_PROGRAMS = keximigratetest

noinst_PROGRAMS = keximigratetest 

noinst_HEADERS = keximigrate.h pqxxmigrate.h pg_type.h

INCLUDES = -I$(top_srcdir)/kexi $(all_includes) -I$(PG_INCDIR) -I$(PQXX_INCDIR)

SUBDIRS = .

METASOURCES = AUTO

keximigratetest_SOURCES = keximigratetest.cpp keximigrate.cpp pqxxmigrate.cpp
keximigratetest_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la -lpqxx $(top_builddir)/kexi/widget/libkexiextendedwidgets.la
keximigratetest_LDFLAGS = $(all_libraries) $(KDE_RPATH) -L$(PQXX_LIBDIR) -L$(PG_LIBDIR) -no-undefined

