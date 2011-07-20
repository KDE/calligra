

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )


########### next target ###############

set(keximigrate_kspread_PART_SRCS kspreadmigrate.cpp )


kde4_add_plugin(keximigrate_kspread ${keximigrate_kspread_PART_SRCS})



target_link_libraries(keximigrate_kspread
    ${KDE4_KPARTS_LIBS}
    keximigrate
    kexidb
    kexiutils
    calligratablescommon
)

install(TARGETS keximigrate_kspread DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximigrate_kspread.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
