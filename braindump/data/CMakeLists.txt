add_subdirectory(states)
add_subdirectory(icons)

### Services ###
install( FILES
    braindump_extensions.desktop
    DESTINATION ${SERVICETYPES_INSTALL_DIR})

### GUI files ###
install( FILES braindumpui.rc braindumpview.rc DESTINATION ${DATA_INSTALL_DIR}/braindump)

install( FILES import/zoom-width.png import/zoom-page.png import/zoom-pixels.png DESTINATION ${DATA_INSTALL_DIR}/braindump/icons/hicolor/22x22/actions)

### desktop files ###
install( FILES braindump.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
