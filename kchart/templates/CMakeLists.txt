
install( FILES  BarChart.chrt Empty.chrt  DESTINATION ${DATA_INSTALL_DIR}/kchart/templates/chart/.source)
install( FILES  .directory BarChart.desktop Empty.desktop  DESTINATION ${DATA_INSTALL_DIR}/kchart/templates/chart)
kde4_install_icons( ${DATA_INSTALL_DIR}/kchart/icons  crystalsvg )

