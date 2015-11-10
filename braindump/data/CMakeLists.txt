add_subdirectory(states)
add_subdirectory(icons)

### Services ###
install( FILES
    braindump_extensions.desktop
    DESTINATION ${SERVICETYPES_INSTALL_DIR})

### GUI files ###
install( FILES braindumpui.rc braindumpview.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/braindump)

### desktop files ###
install( PROGRAMS braindump.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES braindump.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/)
