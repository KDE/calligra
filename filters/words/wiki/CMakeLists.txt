#add_subdirectory(import)

if(SHOULD_BUILD_FILTER_ODT_TO_WIKI)
if (SharedMimeInfo_FOUND)
    install(FILES wiki-format.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
    update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})
endif()

add_subdirectory(export)
endif()
