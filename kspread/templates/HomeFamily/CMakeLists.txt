




########### install files ###############

install( FILES   DESTINATION ${DATA_INSTALL_DIR}/kspread/icons)
install( FILES  BMI.kst MenuPlan.kst VacationChecklist.kst CreditCardTracker.kst  DESTINATION ${DATA_INSTALL_DIR}/kspread/templates/HomeFamily/.source)
install( FILES  .directory BMI.desktop MenuPlan.desktop VacationChecklist.desktop CreditCardTracker.desktop  DESTINATION ${DATA_INSTALL_DIR}/kspread/templates/HomeFamily)





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
