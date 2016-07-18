install( FILES  Worksheet.kst StudentIDCard.kst  DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/templates/General/.source)
install( FILES  .directory Worksheet.desktop StudentIDCard.desktop  DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/templates/General)

ecm_install_icons( ICONS
    48-actions-template_studentidcard.png
    48-actions-template_worksheet.png
    sc-actions-template_studentidcard.svgz
    sc-actions-template_worksheet.svgz

    DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/icons
    THEME hicolor
)
