
install( FILES  TwoColumns.kwt Letter.kwt A4.kwt Memo.kwt TwoColumnsLetter.kwt      ProfessionalLetter.odt ProfessionalA4.odt FaxA4.odt FaxLetter.odt      ColorfulA4.odt ColorfulLetter.odt PlainText.kwt  DESTINATION ${DATA_INSTALL_DIR}/kword/templates/Normal/.source)
install( FILES  .directory TwoColumns.desktop Letter.desktop A4.desktop Memo.desktop      TwoColumnsLetter.desktop ProfessionalLetter.desktop ProfessionalA4.desktop      FaxA4.desktop FaxLetter.desktop      ColorfulA4.desktop ColorfulLetter.desktop  DESTINATION ${DATA_INSTALL_DIR}/kword/templates/Normal)
kde4_install_icons( ${DATA_INSTALL_DIR}/kword/icons  crystalsvg )


