include_directories(
    ${WRITERPERFECT_INCLUDES}
    ${WPD_INCLUDE_DIR}
    ${LIBWPS_INCLUDE_DIR}
)

set(wps2odt_PART_SRCS WPSImport.cpp)

kde4_add_plugin(calligra_filter_wps2odt ${wps2odt_PART_SRCS})

target_link_libraries(calligra_filter_wps2odt komain writerperfect ${LIBWPS_LIBRARIES})

install(TARGETS calligra_filter_wps2odt DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES words_wps_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install(FILES words_wps_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
