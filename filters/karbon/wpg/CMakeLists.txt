
include_directories(${CMAKE_BINARY_DIR}/filters/ ${LIBREVENGE_INCLUDE_DIRS} ${LIBWPG_INCLUDE_DIR}/)

set(wpg2svg_PART_SRCS WPGImport.cpp)

add_library(calligra_filter_wpg2svg MODULE ${wpg2svg_PART_SRCS})

target_link_libraries(calligra_filter_wpg2svg komain ${LIBWPG_LIBRARIES} ${LIBREVENGE_LIBRARIES})
kcoreaddons_desktop_to_json(calligra_filter_wpg2svg calligra_filter_wpg2svg.desktop)

install(TARGETS calligra_filter_wpg2svg DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install(FILES karbon_wpg_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif()
