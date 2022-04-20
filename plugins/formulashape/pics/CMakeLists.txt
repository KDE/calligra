add_subdirectory( hicolor )

ecm_install_icons( ICONS
    32-mimetypes-x-shape-formula.png
    DESTINATION ${KDE_INSTALL_DATADIR}/calligra/icons
    THEME hicolor
)
