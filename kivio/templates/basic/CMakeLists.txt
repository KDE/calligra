########### install files ###############

install( FILES  empty.kft basicflow.kft  DESTINATION ${DATA_INSTALL_DIR}/kivio/templates/Basic/.source)
install( FILES  .directory empty.desktop basicflow.desktop  DESTINATION ${DATA_INSTALL_DIR}/kivio/templates/Basic)

kde4_install_icons( ${DATA_INSTALL_DIR}/kivio/icons crystalsvg )

