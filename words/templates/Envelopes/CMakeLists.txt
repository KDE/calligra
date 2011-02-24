
install( FILES  EnvelopeC6.kwt EnvelopeDL.kwt  DESTINATION ${DATA_INSTALL_DIR}/words/templates/Envelopes/.source/)
install( FILES  .directory EnvelopeC6.desktop EnvelopeDL.desktop  DESTINATION ${DATA_INSTALL_DIR}/words/templates/Envelopes)

kde4_install_icons( ${DATA_INSTALL_DIR}/words/icons  )
