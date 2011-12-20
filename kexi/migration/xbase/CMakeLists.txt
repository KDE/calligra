include_directories( ${XBase_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

########### next target ###############

set(keximigrate_xbase_PART_SRCS xbasemigrate.cpp )

kde4_add_plugin(keximigrate_xbase ${keximigrate_xbase_PART_SRCS})


target_link_libraries(
    keximigrate_xbase
    keximigrate
    kexidb
    kexiutils
    ${KDE4_KPARTS_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${XBase_LIBRARIES} 
    )

install(TARGETS keximigrate_xbase DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximigrate_xbase.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

