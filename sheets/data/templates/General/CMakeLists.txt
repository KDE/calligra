install( FILES  Worksheet.kst StudentIDCard.kst  DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/templates/General/.source)
install( FILES  .directory Worksheet.desktop StudentIDCard.desktop  DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/templates/General)

ecm_install_icons( ICONS
    48-actions-template_studentidcard.png
    48-actions-template_worksheet.png
    sc-actions-template_studentidcard.svgz
    sc-actions-template_worksheet.svgz

    DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/icons
    THEME hicolor
)
