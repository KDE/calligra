add_subdirectory(states)
add_subdirectory(icons)

### Services ###
# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended properties
# braindump_extensions.desktop

### GUI files ###
install( FILES braindumpui.rc braindumpview.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/braindump)

### desktop files ###
install( PROGRAMS org.kde.braindump.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES org.kde.braindump.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
