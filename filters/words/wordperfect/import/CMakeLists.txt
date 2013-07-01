include_directories(
    ${WRITERPERFECT_INCLUDES}
    ${WPD_INCLUDE_DIR}
    ${LIBWPG_INCLUDE_DIR}
)

set(wpd2odt_PART_SRCS WPDImport.cpp )

kde4_add_plugin(calligra_filter_wpd2odt ${wpd2odt_PART_SRCS})

target_link_libraries(calligra_filter_wpd2odt komain writerperfect)

install(TARGETS calligra_filter_wpd2odt DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES words_wpd_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install( FILES words_wpd_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
