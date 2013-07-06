
include_directories(${CMAKE_BINARY_DIR}/filters/ ${WPD_INCLUDE_DIR} ${LIBWPG_INCLUDE_DIR}/)

set(wpg2svg_PART_SRCS WPGImport.cpp)

kde4_add_plugin(calligra_filter_wpg2svg ${wpg2svg_PART_SRCS})

target_link_libraries(calligra_filter_wpg2svg komain ${LIBWPG_LIBRARIES} ${LIBWPG_STREAM_LIBRARIES} ${WPD_LIBRARIES})

install(TARGETS calligra_filter_wpg2svg DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES karbon_wpg_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install(FILES karbon_wpg_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
