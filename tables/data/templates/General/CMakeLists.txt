install( FILES  Worksheet.kst StudentIDCard.kst  DESTINATION ${DATA_INSTALL_DIR}/calligratables/templates/General/.source)
install( FILES  .directory Worksheet.desktop StudentIDCard.desktop  DESTINATION ${DATA_INSTALL_DIR}/calligratables/templates/General)

kde4_install_icons( ${DATA_INSTALL_DIR}/calligratables/icons  )
