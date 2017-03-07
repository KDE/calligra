install( FILES  simple.kgr allcolors.kgr  DESTINATION ${DATA_INSTALL_DIR}/karbon/gradients)
install( FILES  palettes/svg-colors.gpl DESTINATION ${DATA_INSTALL_DIR}/karbon/palettes)
install( FILES  karbon.rc karbon_readonly.rc  DESTINATION ${KXMLGUI_INSTALL_DIR}/karbon)
install( PROGRAMS  org.kde.karbon.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES org.kde.karbon.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install( FILES  karbonrc DESTINATION ${CONFIG_INSTALL_DIR} )

# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended properties
# karbon_viewplugin.desktop karbon_dock.desktop
