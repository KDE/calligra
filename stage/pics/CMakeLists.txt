add_subdirectory( animations )
install( FILES
         webslideshow-sidebar.png
         layout-elements.svg
         DESTINATION ${DATA_INSTALL_DIR}/stage/pics)

ecm_install_icons( ICONS
    16-actions-black.png
    16-actions-highlight.png
    16-actions-pen.png

    16-apps-calligrastage.png
    22-apps-calligrastage.png
    32-apps-calligrastage.png
    48-apps-calligrastage.png
    64-apps-calligrastage.png
    128-apps-calligrastage.png
    256-apps-calligrastage.png
    512-apps-calligrastage.png
    1024-apps-calligrastage.png
    sc-apps-calligrastage.svgz

    DESTINATION ${ICON_INSTALL_DIR}
    THEME hicolor
)
