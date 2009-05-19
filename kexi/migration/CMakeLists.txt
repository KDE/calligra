include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core
  ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_BINARY_DIR}/kexi/widget
  ${CMAKE_SOURCE_DIR}/kexi/main/startup ${CMAKE_BINARY_DIR}/kexi/main
  ${KDE4_INCLUDES}
)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44000)

macro_optional_find_package(MySQL)
macro_log_feature(MYSQL_FOUND "libmysqlclient" "MySQL Client Library" "http://www.mysql.com" FALSE "" "Required by Kexi MySQL Migration driver")
IF (MYSQL_FOUND)
  macro_optional_add_subdirectory( mysql )
ENDIF (MYSQL_FOUND)

macro_optional_find_package(PostgreSQL)
macro_log_feature(POSTGRESQL_FOUND "libpq" "C application programmer's interface to PostgreSQL" "http://www.postgresql.org" FALSE "" "Required by Kexi PostgreSQL Migration driver")

macro_optional_find_package(Pqxx)
macro_log_feature(PQXX_FOUND "libpqxx" "Official C++ client API for PostgreSQL" "http://pqxx.org/development/libpqxx/" FALSE "" "Required by Kexi PostgreSQL Migration driver")

IF (POSTGRESQL_FOUND AND PQXX_FOUND)
  macro_optional_add_subdirectory( pqxx )
ENDIF (POSTGRESQL_FOUND AND PQXX_FOUND)

macro_optional_find_package(FreeTDS)
macro_log_feature(FREETDS_FOUND "FreeTDS" "Open source implementation of the TDS (Tabular Data Stream) protocol" "http://www.freetds.org" FALSE "" "Required by Kexi Sybase Migration driver")
IF(FREETDS_FOUND)
  macro_optional_add_subdirectory( sybase )
ENDIF(FREETDS_FOUND)

macro_optional_find_package(XBase)
macro_log_feature(XBASE_FOUND "XBase" "XBase compatible C++ class library" "http://linux.techass.com/projects/xdb/" FALSE "" "Required by Kexi XBase Migration driver")
IF(XBASE_FOUND)
  macro_optional_add_subdirectory( xbase )
ENDIF(XBASE_FOUND)

#todo add_subdirectory( mdb )

########### next target ###############

set(keximigrate_LIB_SRCS 
   keximigrate.cpp 
   keximigratedata.cpp 
   migratemanager.cpp 
   importwizard.cpp 
   importoptionsdlg.cpp )

kde4_add_library(keximigrate SHARED ${keximigrate_LIB_SRCS})

target_link_libraries(
    keximigrate
    kexicore
    kexidb
    kexiextendedwidgets
    keximain
    kexiutils
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

set_target_properties(keximigrate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS keximigrate  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

set(keximigratetest_SRCS keximigratetest.cpp )


kde4_add_executable(keximigratetest ${keximigratetest_SRCS})

target_link_libraries(keximigratetest  ${KDE4_KDECORE_LIBS} keximigrate kexicore kexidb kexiextendedwidgets keximain )


########### install files ###############

install( FILES  keximigrate.h keximigratedata.h migratemanager.h  DESTINATION ${INCLUDE_INSTALL_DIR}/kexidb COMPONENT Devel)
install( FILES  keximigration_driver.desktop  DESTINATION ${SERVICETYPES_INSTALL_DIR})





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
