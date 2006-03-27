kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kspread/icons FILES  )
install_files( ${DATA_INSTALL_DIR}/kspread/templates/Business/.source FILES  Invoice.kst ExpenseReport.kst BalanceSheet.kst PriceQuotation.kst PackingSlip.kst )
install_files( ${DATA_INSTALL_DIR}/kspread/templates/Business FILES  .directory Invoice.desktop ExpenseReport.desktop BalanceSheet.desktop PriceQuotation.desktop PackingSlip.desktop )

kde4_footer()



#original Makefile.am contents follow:

#template_DATA = .directory Invoice.desktop ExpenseReport.desktop BalanceSheet.desktop PriceQuotation.desktop PackingSlip.desktop
#templatedir = $(kde_datadir)/kspread/templates/Business
#
#templatesrc_DATA = Invoice.kst ExpenseReport.kst BalanceSheet.kst PriceQuotation.kst PackingSlip.kst
#templatesrcdir = $(kde_datadir)/kspread/templates/Business/.source
#
#kspreadicondir = $(kde_datadir)/kspread/icons
#kspreadicon_ICON = AUTO
