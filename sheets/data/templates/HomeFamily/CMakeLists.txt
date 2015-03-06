
install( FILES  BMI.kst MenuPlan.kst VacationChecklist.kst CreditCardTracker.kst  DESTINATION ${DATA_INSTALL_DIR}/sheets/templates/HomeFamily/.source)
install( FILES  .directory BMI.desktop MenuPlan.desktop VacationChecklist.desktop CreditCardTracker.desktop  DESTINATION ${DATA_INSTALL_DIR}/sheets/templates/HomeFamily)

ecm_install_icons( ${DATA_INSTALL_DIR}/sheets/icons  )

