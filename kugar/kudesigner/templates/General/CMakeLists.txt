install( FILES  A0.ktm  A4.ktm  A8.ktm  B10.ktm  B5.ktm  B9.ktm       Executive.ktm  Letter.ktm  A1.ktm  A5.ktm  A9.ktm  B2.ktm   B6.ktm  C5E.ktm      Folio.ktm       A2.ktm  A6.ktm  B0.ktm  B3.ktm   B7.ktm  Comm10E.ktm  Ledger.ktm     Tabloid.ktm  A3.ktm  A7.ktm  B1.ktm  B4.ktm   B8.ktm  DLE.ktm      Legal.ktm  DESTINATION ${DATA_INSTALL_DIR}/kudesigner/templates/General/.source)

install( FILES  .directory A0.desktop  A5.desktop  B0.desktop   B4.desktop  B9.desktop         Folio.desktop      Tabloid.desktop  A1.desktop  A6.desktop  B1.desktop   B5.desktop  C5E.desktop        Ledger.desktop  A2.desktop  A7.desktop  B10.desktop  B6.desktop  Comm10E.desktop    Legal.desktop  A3.desktop  A8.desktop  B2.desktop   B7.desktop  DLE.desktop        Letter.desktop  A4.desktop  A9.desktop  B3.desktop   B8.desktop  Executive.desktop  NPageSize.desktop  DESTINATION ${DATA_INSTALL_DIR}/kudesigner/templates/General)

kde4_install_icons( ${DATA_INSTALL_DIR}/kudesigner/icons  )

