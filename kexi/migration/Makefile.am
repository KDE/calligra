include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkeximigrate.la

INCLUDES = \
	-I$(top_srcdir)/kexi \
	-I$(top_srcdir)/kexi/core \
	-I$(top_srcdir)/kexi/widget \
	-I$(top_srcdir)/kexi/main/startup \
	-I$(top_builddir)/kexi/main/startup \
	$(all_includes)

if compile_pgsql_plugin
pgsql_dir=pqxx
endif

if compile_mysql_plugin
mysql_dir=mysql
endif

SUBDIRS = . $(pgsql_dir) $(mysql_dir)

libkeximigrate_la_METASOURCES = AUTO

libkeximigrate_la_SOURCES = keximigrate.cpp importwizard.cpp migratemanager.cpp \
	keximigratedata.cpp importoptionsdlg.cpp

libkeximigrate_la_LIBADD = \
	$(top_builddir)/kexi/core/libkexicore.la \
	$(top_builddir)/kexi/kexidb/libkexidb.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	$(top_builddir)/kexi/main/libkeximain.la \
	$(LIB_QT) $(LIB_KDECORE)

libkeximigrate_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(VER_INFO)

noinst_HEADERS = importwizard.h migratemanager_p.h

keximigrateincludedir=$(includedir)/kexidb
keximigrateinclude_HEADERS=keximigrate.h keximigratedata.h migratemanager.h

kde_servicetypes_DATA = keximigration_driver.desktop

KDE_OPTIONS=nofinal
noinst_PROGRAMS = keximigratetest

keximigratetest_SOURCES = keximigratetest.cpp
keximigratetest_LDADD = libkeximigrate.la \
	$(top_builddir)/kexi/core/libkexicore.la \
	$(top_builddir)/kexi/kexidb/libkexidb.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	$(top_builddir)/kexi/main/libkeximain.la \
	$(LIB_QT) $(LIB_KDECORE)

keximigratetest_LDFLAGS = $(KDE_RPATH) $(all_libraries)

