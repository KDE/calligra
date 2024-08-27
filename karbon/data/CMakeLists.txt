install( FILES  simple.kgr allcolors.kgr  DESTINATION ${KDE_INSTALL_DATADIR}/karbon/gradients)
install( FILES  palettes/svg-colors.gpl DESTINATION ${KDE_INSTALL_DATADIR}/karbon/palettes)
install( FILES  karbon.rc karbon_readonly.rc  DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/karbon)
install( PROGRAMS  org.kde.calligra.karbon.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install( FILES org.kde.calligra.karbon.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.calligra.karbon.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)
install( FILES  karbonrc DESTINATION ${KDE_INSTALL_CONFDIR} )

# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended properties
# karbon_viewplugin.desktop karbon_dock.desktop
