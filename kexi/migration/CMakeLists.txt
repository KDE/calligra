kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/main/startup ${KDE4_INCLUDES}  )

INCLUDE (FindMySQL)
IF (MYSQL_FOUND)
   add_subdirectory( mysql )
ENDIF (MYSQL_FOUND)

INCLUDE (FindPostgreSQL)
INCLUDE (FindPqxx)
IF (POSTGRESQL_FOUND AND PQXX_FOUND)
   add_subdirectory( pqxx )
ENDIF (POSTGRESQL_FOUND AND PQXX_FOUND)

########### next target ###############

set(keximigrate_LIB_SRCS 
   keximigrate.cpp 
   importwizard.cpp 
   migratemanager.cpp 
   keximigratedata.cpp 
   importoptionsdlg.cpp )

kde4_automoc(${keximigrate_LIB_SRCS})

kde4_add_library(keximigrate SHARED ${keximigrate_LIB_SRCS})

target_link_libraries(keximigrate  ${KDE4_KDECORE_LIBS} kexicore kexidb kexiextendedwidgets keximain )

set_target_properties(keximigrate PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} keximigrate )


########### next target ###############

set(keximigratetest_SRCS keximigratetest.cpp )

kde4_automoc(${keximigratetest_SRCS})

kde4_add_executable(keximigratetest ${keximigratetest_SRCS})

target_link_libraries(keximigratetest  ${KDE4_KDECORE_LIBS} keximigrate kexicore kexidb kexiextendedwidgets keximain )


########### install files ###############

install_files( /include/kexidb FILES  keximigrate.h keximigratedata.h migratemanager.h )
install_files( ${SERVICETYPES_INSTALL_DIR} FILES  keximigration_driver.desktop )

kde4_footer()



#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#lib_LTLIBRARIES = libkeximigrate.la
#
#INCLUDES = \
#	-I$(top_srcdir)/kexi \
#	-I$(top_srcdir)/kexi/core \
#	-I$(top_srcdir)/kexi/widget \
#	-I$(top_srcdir)/kexi/main/startup \
#	-I$(top_builddir)/kexi/main/startup \
#	$(all_includes)
#
#if compile_pgsql_plugin
#pgsql_dir=pqxx
#endif
#
#if compile_mysql_plugin
#mysql_dir=mysql
#endif
#
#SUBDIRS = . $(pgsql_dir) $(mysql_dir)
#
#libkeximigrate_la_METASOURCES = AUTO
#
#libkeximigrate_la_SOURCES = keximigrate.cpp importwizard.cpp migratemanager.cpp \
#	keximigratedata.cpp importoptionsdlg.cpp
#
#libkeximigrate_la_LIBADD = \
#	$(top_builddir)/kexi/core/libkexicore.la \
#	$(top_builddir)/kexi/kexidb/libkexidb.la \
#	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
#	$(top_builddir)/kexi/main/libkeximain.la \
#	$(LIB_QT) $(LIB_KDECORE)
#
#libkeximigrate_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(VER_INFO)
#
#noinst_HEADERS = importwizard.h migratemanager_p.h
#
#keximigrateincludedir=$(includedir)/kexidb
#keximigrateinclude_HEADERS=keximigrate.h keximigratedata.h migratemanager.h
#
#kde_servicetypes_DATA = keximigration_driver.desktop
#
#KDE_OPTIONS=nofinal
#noinst_PROGRAMS = keximigratetest
#
#keximigratetest_SOURCES = keximigratetest.cpp
#keximigratetest_LDADD = libkeximigrate.la \
#	$(top_builddir)/kexi/core/libkexicore.la \
#	$(top_builddir)/kexi/kexidb/libkexidb.la \
#	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
#	$(top_builddir)/kexi/main/libkeximain.la \
#	$(LIB_QT) $(LIB_KDECORE)
#
#keximigratetest_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#
