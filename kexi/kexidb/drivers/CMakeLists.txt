

add_subdirectory( sqlite )
#ancient add_subdirectory( sqlite2 )

INCLUDE (FindMySQL)
IF (MYSQL_FOUND)
   add_subdirectory( mySQL )
ENDIF (MYSQL_FOUND)

INCLUDE (FindPostgreSQL)
INCLUDE (FindPqxx)
IF (POSTGRESQL_FOUND AND PQXX_FOUND)
   add_subdirectory( pqxx )
ENDIF (POSTGRESQL_FOUND AND PQXX_FOUND)

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
