########### install files ###############

install( FILES  empty.otg  DESTINATION ${DATA_INSTALL_DIR}/kivio/templates/Basic/.source)
install( FILES  .directory empty.desktop  DESTINATION ${DATA_INSTALL_DIR}/kivio/templates/Basic)

kde4_install_icons( ${DATA_INSTALL_DIR}/kivio/icons  )

