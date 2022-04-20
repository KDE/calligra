ecm_install_icons( ICONS
    16-actions-snap-bounding-box.png
    16-actions-snap-extension.png
    22-actions-format-convert-to-path.png
    22-actions-createpath.png
    22-actions-editpath.png
    22-actions-hand.png
    22-actions-format-break-node.png
    22-actions-format-disconnect-node.png
    22-actions-node-type-cusp.png
    22-actions-format-node-curve.png
    22-actions-format-insert-node.png
    22-actions-format-connect-node.png
    22-actions-format-node-line.png
    22-actions-format-join-node.png
    22-actions-format-remove-node.png
    22-actions-node-type-smooth.png
    22-actions-node-type-symmetric.png
    22-actions-format-segment-curve.png
    22-actions-format-segment-line.png
    22-actions-pathshape.png
    sc-actions-snap-bounding-box.svg
    sc-actions-snap-extension.svg
    DESTINATION ${KDE_INSTALL_DATADIR}/calligra/icons
    THEME hicolor
)

install( FILES
    zoom_in_cursor.png
    zoom_out_cursor.png
    DESTINATION ${KDE_INSTALL_DATADIR}/calligra/cursors
)

install( FILES
    questionmark.png
    DESTINATION ${KDE_INSTALL_DATADIR}/calligra/pics
)
