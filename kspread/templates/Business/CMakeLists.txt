
install( FILES  Invoice.kst ExpenseReport.kst BalanceSheet.kst PriceQuotation.kst PackingSlip.kst  DESTINATION ${DATA_INSTALL_DIR}/kspread/templates/Business/.source)
install( FILES  .directory Invoice.desktop ExpenseReport.desktop BalanceSheet.desktop PriceQuotation.desktop PackingSlip.desktop  DESTINATION ${DATA_INSTALL_DIR}/kspread/templates/Business)

kde4_install_icons( ${DATA_INSTALL_DIR}/kspread/icons  )

