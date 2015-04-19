include_directories( ${PostgreSQL_INCLUDE_DIRS} ${PQXX_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

add_definitions( ${KDE4_ENABLE_EXCEPTIONS} )

########### next target ###############

set(keximigrate_pqxx_PART_SRCS pqxxmigrate.cpp )

kde4_add_plugin(keximigrate_pqxx ${keximigrate_pqxx_PART_SRCS})



target_link_libraries(
    keximigrate_pqxx 
    ${POSTGRESQL_LIBRARIES} 
    ${PQXX_LIBRARIES} 
    ${KDE4_KDECORE_LIBS} 
    keximigrate 
    kexidb
    kexiutils
    ${CALLIGRADB_LIBS}
)

install(TARGETS keximigrate_pqxx  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximigrate_pqxx.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
