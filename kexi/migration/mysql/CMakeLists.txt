include_directories( ${MYSQL_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi   )

########### next target ###############

set(keximigrate_mysql_PART_SRCS mysqlmigrate.cpp )

add_library(keximigrate_mysql MODULE ${keximigrate_mysql_PART_SRCS})



target_link_libraries(
    keximigrate_mysql
    keximigrate
    kexiutils

    KDb

    ${MYSQL_LIBRARIES}
    )

install(TARGETS keximigrate_mysql  DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
