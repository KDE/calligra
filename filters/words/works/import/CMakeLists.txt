include_directories(
    ../../../writerperfect
    ${WPD_INCLUDE_DIR}
    ${LIBWPS_INCLUDE_DIR}
)

set(wpsimport_PART_SRCS WPSImport.cpp)

kde4_add_plugin(wpsimport ${wpsimport_PART_SRCS})

target_link_libraries(wpsimport komain writerperfect ${LIBWPS_LIBRARIES})

install(TARGETS wpsimport DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES words_wps_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES words_wps_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
