
install( FILES  Invoice.kst ExpenseReport.kst BalanceSheet.kst PriceQuotation.kst PackingSlip.kst  DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/templates/Business/.source)
install( FILES  .directory Invoice.desktop ExpenseReport.desktop BalanceSheet.desktop PriceQuotation.desktop PackingSlip.desktop  DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/templates/Business)

ecm_install_icons( ICONS
    48-actions-template_balancesheet.png
    48-actions-template_expensereport.png
    48-actions-template_invoice.png
    48-actions-template_packingslip.png
    48-actions-template_pricequotation.png
    sc-actions-template_balancesheet.svgz
    sc-actions-template_expensereport.svgz
    sc-actions-template_invoice.svgz
    sc-actions-template_packingslip.svgz
    sc-actions-template_pricequotation.svgz

    DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/icons
    THEME hicolor
)

