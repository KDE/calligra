include_directories( ${FreeTDS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

########### next target ###############

set(keximigrate_sybase_PART_SRCS sybasemigrate.cpp )

kde4_add_plugin(keximigrate_sybase ${keximigrate_sybase_PART_SRCS})



target_link_libraries(keximigrate_sybase ${FreeTDS_LIBRARIES} ${KDE4_KDECORE_LIBS}
                      kexiutils kexidb keximigrate ${CALLIGRADB_LIBS})

install(TARGETS keximigrate_sybase  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximigrate_sybase.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
