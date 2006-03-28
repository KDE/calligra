kde4_header()

add_subdirectory( sqlite )
add_subdirectory( sqlite2 )

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(pgsql_dir)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(mysql_dir)")



########### install files ###############


kde4_footer()



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
