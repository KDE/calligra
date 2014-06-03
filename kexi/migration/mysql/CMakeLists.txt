include_directories( ${MYSQL_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

########### next target ###############

set(keximigrate_mysql_PART_SRCS mysqlmigrate.cpp )

kde4_add_plugin(keximigrate_mysql ${keximigrate_mysql_PART_SRCS})



target_link_libraries(
    keximigrate_mysql
    keximigrate
    kexidb 
    kexiutils
    ${CALLIGRADB_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${MYSQL_LIBRARIES}
    )

install(TARGETS keximigrate_mysql  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximigrate_mysql.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
