########### install files ###############

install( FILES  empty-portrait.otg empty-landscape.otg
         DESTINATION ${DATA_INSTALL_DIR}/flow/templates/Basic/.source)
install( FILES  empty-portrait.desktop empty-landscape.desktop
         DESTINATION ${DATA_INSTALL_DIR}/flow/templates/Basic)

kde4_install_icons( ${DATA_INSTALL_DIR}/flow/icons  )
