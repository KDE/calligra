include_directories(
    ../../../outputhelper/
    ${LIBODFGEN_INCLUDE_DIRS}
    ${LIBETONYEK_INCLUDE_DIRS}
    ${LIBREVENGE_INCLUDE_DIRS}
    ${KOMAIN_INCLUDES}
)

set(key2odp_PART_SRCS
    KeyImport.cpp
    ../../../outputhelper/StringDocumentHandler.cxx
    ../../../outputhelper/FemtoZip.cxx
    ../../../outputhelper/OutputFileHelper.cxx
)

add_library(calligra_filter_key2odp MODULE ${key2odp_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_key2odp calligra_filter_key2odp.desktop)

target_link_libraries(calligra_filter_key2odp komain ${LIBREVENGE_LIBRARIES} ${LIBODFGEN_LIBRARIES} ${LIBETONYEK_LIBRARIES})

install(TARGETS calligra_filter_key2odp DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install(FILES stage_key_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif()

if (SHARED_MIME_INFO_FOUND)
    install(FILES x-iwork-keynote-sffkey.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
    update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})
endif ()
