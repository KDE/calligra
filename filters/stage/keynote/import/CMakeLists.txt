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

kde4_add_plugin(calligra_filter_key2odp ${key2odp_PART_SRCS})

target_link_libraries(calligra_filter_key2odp komain ${LIBREVENGE_LIBRARIES} ${LIBODFGEN_LIBRARIES} ${LIBETONYEK_LIBRARIES})

install(TARGETS calligra_filter_key2odp DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES calligra_filter_key2odp.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install(FILES stage_key_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
endif(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)

if (SHARED_MIME_INFO_FOUND)
    install(FILES x-iwork-keynote-sffkey.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
    update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})
endif (SHARED_MIME_INFO_FOUND)
