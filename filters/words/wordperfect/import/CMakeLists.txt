include_directories(
    ../../../writerperfect
    ${WPD_INCLUDE_DIR}
    ${LIBWPG_INCLUDE_DIR}
)

set(wpdimport_PART_SRCS WPDImport.cpp )


kde4_add_plugin(wpdimport ${wpdimport_PART_SRCS})



target_link_libraries(wpdimport komain writerperfect)

install(TARGETS wpdimport DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES words_wpd_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})

install( FILES words_wpd_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})


