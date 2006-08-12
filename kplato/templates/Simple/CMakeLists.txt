install( FILES  Plain.kplatot 8HourDay-40HourWeek.kplatot  DESTINATION ${DATA_INSTALL_DIR}/kplato/templates/Simple/.source)
install( FILES  .directory Plain.desktop 8HourDay-40HourWeek.desktop  DESTINATION ${DATA_INSTALL_DIR}/kplato/templates/Simple)

kde4_install_icons( ${DATA_INSTALL_DIR}/kplato/icons crystalsvg )
