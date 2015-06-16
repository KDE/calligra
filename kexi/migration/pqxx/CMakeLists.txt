include_directories( ${PostgreSQL_INCLUDE_DIRS} ${PQXX_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi   )

kde_enable_exceptions()

########### next target ###############

set(keximigrate_pqxx_PART_SRCS pqxxmigrate.cpp )

add_library(keximigrate_pqxx MODULE ${keximigrate_pqxx_PART_SRCS})



target_link_libraries(
    keximigrate_pqxx
    ${POSTGRESQL_LIBRARIES}
    ${PQXX_LIBRARIES}

    keximigrate
    kexiutils

    KDb
)

install(TARGETS keximigrate_pqxx  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

