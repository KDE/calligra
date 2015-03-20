include_directories( ${FreeTDS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi   )

########### next target ###############

set(keximigrate_sybase_PART_SRCS sybasemigrate.cpp )

add_library(keximigrate_sybase MODULE ${keximigrate_sybase_PART_SRCS})



target_link_libraries(keximigrate_sybase ${FreeTDS_LIBRARIES} 
                      kexiutils kexidb keximigrate ${CALLIGRADB_LIBS})

install(TARGETS keximigrate_sybase  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximigrate_sybase.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
