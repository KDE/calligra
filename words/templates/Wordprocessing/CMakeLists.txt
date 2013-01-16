
install( FILES  A4.odt ProfessionalLetter.odt ProfessionalA4.odt FaxA4.odt FaxLetter.odt      ColorfulA4.odt ColorfulLetter.odt   DESTINATION ${DATA_INSTALL_DIR}/words/templates/Normal/.source)
install( FILES  .directory A4.desktop  ProfessionalLetter.desktop ProfessionalA4.desktop      FaxA4.desktop FaxLetter.desktop      ColorfulA4.desktop ColorfulLetter.desktop  DESTINATION ${DATA_INSTALL_DIR}/words/templates/Normal)
kde4_install_icons( ${DATA_INSTALL_DIR}/words/icons   )


