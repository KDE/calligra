

include_directories( ${SQLITE_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( -D__KEXIDB__= )

set(KEXI_SQLITE_DUMP_TOOL kexi_sqlite3_dump)
ADD_DEFINITIONS( -DKEXI_SQLITE_DUMP_TOOL=\\"${KEXI_SQLITE_DUMP_TOOL}\\" )

########### next target ###############

set(kexidb_sqlite3driver_PART_SRCS 
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

target_link_libraries(kexidb_sqlite3driver ${KDE4_KPARTS_LIBS} ${SQLITE_LIBRARIES} kexidb kexiutils )

install(TARGETS kexidb_sqlite3driver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_sqlite3driver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

add_subdirectory(icu)
add_subdirectory(dump)
