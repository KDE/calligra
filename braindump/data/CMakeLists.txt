add_subdirectory(states)
add_subdirectory(icons)

### GUI files ###
install( FILES braindumpui.rc braindumpview.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/braindump)

install( FILES import/zoom-pixels.png DESTINATION ${DATA_INSTALL_DIR}/braindump/icons/hicolor/22x22/actions)

### desktop files ###
install( PROGRAMS braindump.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES braindump.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/)
