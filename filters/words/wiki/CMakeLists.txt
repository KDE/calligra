#add_subdirectory(import)

install(FILES wiki-format.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})

add_subdirectory(export)
