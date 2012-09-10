add_subdirectory( animations )
install( FILES
         webslideshow-sidebar.png
         layout-elements.svg
         DESTINATION ${DATA_INSTALL_DIR}/stage/pics)

kde4_install_icons( ${ICON_INSTALL_DIR} )

