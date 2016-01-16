include_directories(
    ../../../outputhelper/
    ${LIBODFGEN_INCLUDE_DIRS}
    ${LIBREVENGE_INCLUDE_DIRS}
    ${LIBWPS_INCLUDE_DIRS}
    ${KOMAIN_INCLUDES}
)

set(wps2odt_PART_SRCS
    WPSImport.cpp
    ../../../outputhelper/StringDocumentHandler.cxx
    ../../../outputhelper/FemtoZip.cxx
    ../../../outputhelper/OutputFileHelper.cxx
)

add_library(calligra_filter_wps2odt MODULE ${wps2odt_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_wps2odt calligra_filter_wps2odt.desktop)

target_link_libraries(calligra_filter_wps2odt komain ${LIBODFGEN_LIBRARIES} ${LIBREVENGE_LIBRARIES} ${LIBWPS_LIBRARIES})

install(TARGETS calligra_filter_wps2odt DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install(FILES words_wps_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif()
