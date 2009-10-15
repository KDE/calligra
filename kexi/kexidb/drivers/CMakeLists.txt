
# drivers will be moved out of this dir in Predicate, and then area will be redefined
#remove_definitions(-DKDE_DEFAULT_DEBUG_AREA)
#add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44001)

add_subdirectory( sqlite )
#ancient add_subdirectory( sqlite2 )

INCLUDE (FindMySQL)
IF (MYSQL_FOUND)
   add_subdirectory( mySQL )
ENDIF (MYSQL_FOUND)

INCLUDE (FindPkgConfig)
INCLUDE (FindPostgreSQL)

set (PQXX_MIN_VERSION "3.0.0")
set (PQXX_MAX_VERSION "4.0.0")

macro_optional_find_package(PostgreSQL)
pkg_check_modules (PQXX libpqxx)

IF (PQXX_FOUND AND PQXX_VERSION VERSION_GREATER PQXX_MIN_VERSION AND PQXX_VERSION VERSION_LESS PQXX_MAX_VERSION)
   add_subdirectory( pqxx )
ENDIF (PQXX_FOUND AND PQXX_VERSION VERSION_GREATER PQXX_MIN_VERSION AND PQXX_VERSION VERSION_LESS PQXX_MAX_VERSION)

INCLUDE(FindFreeTDS)
IF (FreeTDS_FOUND)
   add_subdirectory(sybase)
ENDIF (FreeTDS_FOUND)

INCLUDE(FindXBase)
IF(XBASE_FOUND)
   add_subdirectory(xbase)
ENDIF(XBASE_FOUND)

#message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(pgsql_dir)")
#message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(mysql_dir)")



########### install files ###############






#original Makefile.am contents follow:

#if compile_mysql_plugin
#mysql_dir=mySQL
#endif
#
#
#if compile_pgsql_plugin
#pgsql_dir=pqxx
#endif
#
#SUBDIRS =  sqlite sqlite2 $(pgsql_dir) $(mysql_dir)
#
