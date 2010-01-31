
# definitins used for the sqlite driver and the shell
ADD_DEFINITIONS(
    # sqlite compile-time options, http://sqlite.org/compile.html
    -DSQLITE_SECURE_DELETE
    -DSQLITE_ENABLE_COLUMN_METADATA # Some additional APIs that provide convenient access to meta-data
                                    # about tables and queries
    -DSQLITE_ENABLE_FTS3 # Version 3 of the full-text search engine
    -DSQLITE_ENABLE_FTS3_PARENTHESIS # Modifies the query pattern parser in FTS3 such that it supports
                                     # operators AND and NOT (in addition to the usual OR and NEAR)
                                     # and also allows query expressions to contain nested parenthesesis.
    -DSQLITE_ENABLE_MEMORY_MANAGEMENT # Extra logic to SQLite that allows it to release unused memory upon request
    -DSQLITE_ENABLE_RTREE # Support for the R*Tree index extension
    -DSQLITE_ENABLE_STAT2 # Additional logic to the ANALYZE command and to the query planner that can help SQLite
                          # to chose a better query plan under certain situations
    -DSQLITE_ENABLE_UPDATE_DELETE_LIMIT # Optional ORDER BY and LIMIT clause on UPDATE and DELETE statements
    -DSQLITE_ENABLE_UNLOCK_NOTIFY # Enables the sqlite3_unlock_notify() interface and its associated functionality
                                  # (http://sqlite.org/unlock_notify.html)
    -DSQLITE_SOUNDEX # Enables the soundex() SQL function (http://sqlite.org/lang_corefunc.html#soundex)

# todo -DSQLITE_OMIT_DEPRECATED
)

add_subdirectory( 3rdparty/sqlite3 )

# sqlite driver's definitions
ADD_DEFINITIONS(
    -D__KEXIDB__
)

include_directories( 3rdparty/sqlite3 ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES} )

set(kexidb_sqlite3driver_PART_SRCS
   3rdparty/sqlite3/sqlite3.c

   sqliteconnection.cpp 
   sqlitedriver.cpp 
   sqlitecursor.cpp 
   sqlitekeywords.cpp 
   sqlitepreparedstatement.cpp
   sqliteadmin.cpp
   sqlitevacuum.cpp
   sqlitealter.cpp
)

kde4_add_plugin(kexidb_sqlite3driver ${kexidb_sqlite3driver_PART_SRCS})

target_link_libraries(kexidb_sqlite3driver ${QT_QT3SUPPORT_LIBRARY} ${KDE4_KPARTS_LIBS} kexidb kexiutils ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS} )

install(TARGETS kexidb_sqlite3driver  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  kexidb_sqlite3driver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
