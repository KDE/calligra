add_subdirectory(states)
add_subdirectory(icons)

### Services ###
# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended properties
# braindump_extensions.desktop

### GUI files ###
install( FILES braindumpui.rc braindumpview.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/braindump)

### desktop files ###
install( PROGRAMS org.kde.braindump.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install( FILES org.kde.braindump.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
