if compile_mysql_plugin
mysql_dir=mySQL
endif


if compile_pgsql_plugin
pgsql_dir=pqxx
endif

#piggz: please port the driver to newer API
#SUBDIRS =  sqlite  $(pgsql_dir) $(mysql_dir)
SUBDIRS =  sqlite $(mysql_dir)

