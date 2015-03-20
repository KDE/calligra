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
    kexidb
    kexiutils
    ${CALLIGRADB_LIBS}
)

install(TARGETS keximigrate_pqxx  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximigrate_pqxx.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
