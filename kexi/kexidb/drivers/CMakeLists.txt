
# drivers will be moved out of this dir in Predicate, and then area will be redefined
#remove_definitions(-DKDE_DEFAULT_DEBUG_AREA)
#add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44001)

# always available
add_subdirectory( sqlite )

INCLUDE (FindMySQL)
IF (MYSQL_FOUND)
   add_subdirectory( mySQL )
ENDIF (MYSQL_FOUND)

INCLUDE (FindPostgreSQL)

set (PQXX_MIN_VERSION "3.0.0")
set (PQXX_MAX_VERSION "4.0.0")

macro_optional_find_package(PostgreSQL)
pkg_check_modules (PQXX libpqxx)

IF (POSTGRESQL_FOUND AND PQXX_FOUND AND PQXX_VERSION VERSION_GREATER PQXX_MIN_VERSION AND PQXX_VERSION VERSION_LESS PQXX_MAX_VERSION)
   add_subdirectory( pqxx )
ENDIF (POSTGRESQL_FOUND AND PQXX_FOUND AND PQXX_VERSION VERSION_GREATER PQXX_MIN_VERSION AND PQXX_VERSION VERSION_LESS PQXX_MAX_VERSION)

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
