




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kspread/icons FILES  )
install_files( ${DATA_INSTALL_DIR}/kspread/templates/HomeFamily/.source FILES  BMI.kst MenuPlan.kst VacationChecklist.kst CreditCardTracker.kst )
install_files( ${DATA_INSTALL_DIR}/kspread/templates/HomeFamily FILES  .directory BMI.desktop MenuPlan.desktop VacationChecklist.desktop CreditCardTracker.desktop )





#original Makefile.am contents follow:

#template_DATA = .directory BMI.desktop MenuPlan.desktop VacationChecklist.desktop CreditCardTracker.desktop
#templatedir = $(kde_datadir)/kspread/templates/HomeFamily
#
#templatesrc_DATA = BMI.kst MenuPlan.kst VacationChecklist.kst CreditCardTracker.kst
#templatesrcdir = $(kde_datadir)/kspread/templates/HomeFamily/.source
#
#kspreadicondir = $(kde_datadir)/kspread/icons
#kspreadicon_ICON = AUTO
#
