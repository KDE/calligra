
install( FILES  BMI.kst MenuPlan.kst VacationChecklist.kst CreditCardTracker.kst  DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/templates/HomeFamily/.source)
install( FILES  .directory BMI.desktop MenuPlan.desktop VacationChecklist.desktop CreditCardTracker.desktop  DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/templates/HomeFamily)

ecm_install_icons( ICONS
    48-actions-template_bmi.png
    48-actions-template_creditcardtracker.png
    48-actions-template_menuplan.png
    48-actions-template_vacationchecklist.png
    sc-actions-template_bmi.svgz
    sc-actions-template_creditcardtracker.svgz
    sc-actions-template_menuplan.svgz
    sc-actions-template_vacationchecklist.svgz

    DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/icons
    THEME hicolor
)

