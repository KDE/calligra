########### install files ###############

install( FILES  empty.otg  DESTINATION ${DATA_INSTALL_DIR}/flow/templates/Basic/.source)
install( FILES  empty.desktop  DESTINATION ${DATA_INSTALL_DIR}/flow/templates/Basic)

kde4_install_icons( ${DATA_INSTALL_DIR}/flow/icons  )
