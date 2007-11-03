
FILE(GLOB datafiles *.xpm)
FILE(GLOB shapefiles *.shape)
FILE(GLOG pngfiles *.png)
install( FILES  ${datafiles} ${shapefiles} ${pngfiles} desc DESTINATION
${DATA_INSTALL_DIR}/kivio/stencils/Dia/Network)




