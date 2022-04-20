add_subdirectory( animations )

install( FILES
         layout-elements.svg
         DESTINATION ${KDE_INSTALL_DATADIR}/calligrastage/pics)

install( FILES
    pen.png
    DESTINATION ${KDE_INSTALL_DATADIR}/calligrastage/cursors
)

ecm_install_icons( ICONS
    16-actions-pause-to-black-screen.png
    16-actions-highlight-pointer-spot.png

    DESTINATION ${KDE_INSTALL_DATADIR}/calligrastage/icons
    THEME hicolor
)

ecm_install_icons( ICONS
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

    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME hicolor
)
