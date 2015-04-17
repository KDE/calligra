install( FILES  Empty.planworkt  DESTINATION ${DATA_INSTALL_DIR}/planwork/templates/Simple/.source)
install( FILES  .directory Empty.desktop DESTINATION ${DATA_INSTALL_DIR}/planwork/templates/Simple)

ecm_install_icons( ICONS
    16-actions-template_empty.png
    22-actions-template_empty.png
    32-actions-template_empty.png
    48-actions-template_empty.png
    64-actions-template_empty.png
    128-actions-template_empty.png

    DESTINATION ${DATA_INSTALL_DIR}/planwork/icons
    THEME hicolor
)
