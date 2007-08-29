FILE(GLOB datafiles *.png)
FILE(GLOB shapefiles *.shape)
install( FILES  ${datafiles} ${shapefiles} desc icon.xpm  DESTINATION ${DATA_INSTALL_DIR}/kivio/stencils/Dia/Assorted)

