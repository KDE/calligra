include_directories(
    ../../../outputhelper/
    ${LIBODFGEN_INCLUDE_DIRS}
    ${LIBREVENGE_INCLUDE_DIRS}
    ${LIBWPG_INCLUDE_DIRS}
    ${KOMAIN_INCLUDES}
)

set(wpg2odg_PART_SRCS
    WPGImport.cpp
    ../../../outputhelper/StringDocumentHandler.cxx
    ../../../outputhelper/FemtoZip.cxx
    ../../../outputhelper/OutputFileHelper.cxx
)

add_library(calligra_filter_wpg2odg MODULE ${wpg2odg_PART_SRCS})
calligra_filter_desktop_to_json(calligra_filter_wpg2odg calligra_filter_wpg2odg.desktop)

target_link_libraries(calligra_filter_wpg2odg komain ${LIBODFGEN_LIBRARIES} ${LIBREVENGE_LIBRARIES} ${LIBWPG_LIBRARIES})

install(TARGETS calligra_filter_wpg2odg DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install(FILES flow_wpg_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif()
