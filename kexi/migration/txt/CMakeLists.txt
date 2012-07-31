

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )


########### next target ###############

set(keximigrate_txtmigrate_PART_SRCS txtmigrate.cpp )


kde4_add_plugin(keximigrate_txt ${keximigrate_txtmigrate_PART_SRCS})



target_link_libraries(keximigrate_txt  
    ${KDE4_KPARTS_LIBS} 
    keximigrate
    kexidb
    kexiutils
    ${CALLIGRADB_LIBS}
)

install(TARGETS keximigrate_txt  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximigrate_txt.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
