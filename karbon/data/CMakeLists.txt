install( FILES  simple.kgr allcolors.kgr  DESTINATION ${DATA_INSTALL_DIR}/karbon/gradients)
install( FILES  karbon_module.desktop  DESTINATION ${SERVICETYPES_INSTALL_DIR})
install( FILES  karbonpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  karbon.rc karbon_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon)
install( FILES  karbon.desktop  DESTINATION ${XDG_APPS_DIR})

