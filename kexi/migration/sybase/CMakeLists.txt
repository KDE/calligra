include_directories( ${FreeTDS_INCLUDE_DIR} )

########### next target ###############

set(keximigrate_sybase_PART_SRCS sybasemigrate.cpp )

add_library(keximigrate_sybase MODULE ${keximigrate_sybase_PART_SRCS})



target_link_libraries(keximigrate_sybase ${FreeTDS_LIBRARIES}
                      kexiutils keximigrate KDb)

install(TARGETS keximigrate_sybase  DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
