#add_subdirectory(import)

if(SHOULD_BUILD_FILTER_ODT_TO_WIKI)
if (SharedMimeInfo_FOUND)
    install(FILES wiki-format.xml DESTINATION ${KDE_INSTALL_MIMEDIR})
    update_xdg_mimetypes(${KDE_INSTALL_MIMEDIR})
endif()

add_subdirectory(export)
endif()
