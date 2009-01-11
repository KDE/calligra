
add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44001)

add_subdirectory( sqlite )
#ancient add_subdirectory( sqlite2 )

INCLUDE (FindMySQL)
IF (MYSQL_FOUND)
   add_subdirectory( mySQL )
ENDIF (MYSQL_FOUND)

INCLUDE (FindPkgConfig)
INCLUDE (FindPostgreSQL)
INCLUDE (FindPqxx)
pkg_check_modules (PQXX libpqxx)
IF (POSTGRESQL_FOUND AND PQXX_FOUND AND PQXX_VERSION>=2.6 AND PQXX_VERSION<3)
   add_subdirectory( pqxx )
ENDIF (POSTGRESQL_FOUND AND PQXX_FOUND AND PQXX_VERSION>=2.6 AND PQXX_VERSION<3)

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
