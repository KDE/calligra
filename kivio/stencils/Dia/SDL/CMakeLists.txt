FILE(GLOB datafiles *.xpm)
FILE(GLOB shapefiles *.shape)
install( FILES  ${datafiles} ${shapefiles} desc DESTINATION
${DATA_INSTALL_DIR}/kivio/stencils/Dia/SDL)
