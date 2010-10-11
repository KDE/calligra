

file(GLOB desktop_FILES *.desktop)
file(GLOB otp_FILES *.otp)

install( FILES ${desktop_FILES} DESTINATION ${DATA_INSTALL_DIR}/kpresenter/templates/odf/ )
install( FILES ${otp_FILES} DESTINATION ${DATA_INSTALL_DIR}/kpresenter/templates/odf/.source)
install( FILES .directory DESTINATION ${DATA_INSTALL_DIR}/kpresenter/templates/odf/)

kde4_install_icons( ${DATA_INSTALL_DIR}/kpresenter/icons  )
