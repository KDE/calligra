
install( FILES  A4.odt ProfessionalLetter.odt ProfessionalA4.odt FaxA4.odt FaxLetter.odt      ColorfulA4.odt ColorfulLetter.odt   DESTINATION ${DATA_INSTALL_DIR}/words/templates/Normal/.source)
install( FILES  a4.png professional.png fax.png colorful.png   DESTINATION ${DATA_INSTALL_DIR}/words/templates/Normal/.thumbnail)
install( FILES  .directory A4.desktop  ProfessionalLetter.desktop ProfessionalA4.desktop      FaxA4.desktop FaxLetter.desktop      ColorfulA4.desktop ColorfulLetter.desktop  DESTINATION ${DATA_INSTALL_DIR}/words/templates/Normal)

ecm_install_icons( ICONS
    48-actions-template_a4.png
    sc-actions-template_a4.svgz

    48-actions-template_colorful.png
    128-actions-template_colorful.png

    48-actions-template_fax.png
    sc-actions-template_fax.svgz

    48-actions-template_letter.png
    sc-actions-template_letter.svgz

    48-actions-template_plaintext.png

    DESTINATION ${DATA_INSTALL_DIR}/words/icons
    THEME hicolor
)


