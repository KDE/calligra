
install( FILES  empty.odg  DESTINATION ${KDE_INSTALL_DATADIR}/karbon/templates/Basic/.source)
install( FILES  .directory empty.desktop  DESTINATION ${KDE_INSTALL_DATADIR}/karbon/templates/Basic)

file(GLOB karbon_template_icons "${CMAKE_CURRENT_SOURCE_DIR}/*-actions-*.png")
ecm_install_icons(ICONS ${karbon_template_icons} DESTINATION ${KDE_INSTALL_DATADIR}/karbon/icons)

