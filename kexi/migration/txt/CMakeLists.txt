

include_directories( ${CMAKE_SOURCE_DIR}/kexi   )


########### next target ###############

set(keximigrate_txtmigrate_PART_SRCS txtmigrate.cpp )


add_library(keximigrate_txt MODULE ${keximigrate_txtmigrate_PART_SRCS})



target_link_libraries(keximigrate_txt  
    keximigrate
    kexidb
    kexiutils
    ${CALLIGRADB_LIBS}
)

install(TARGETS keximigrate_txt  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximigrate_txt.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
