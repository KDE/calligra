




########### install files ###############

install( FILES  BusinessCards10.kwt LabelsL16.kwt  DESTINATION ${DATA_INSTALL_DIR}/kword/templates/CardsAndLabels/)
install( FILES  .directory BusinessCards10.desktop LabelsL16.desktop  DESTINATION ${DATA_INSTALL_DIR}/kword/templates/CardsAndLabels)
install( FILES   DESTINATION ${DATA_INSTALL_DIR}/kword/icons)





#original Makefile.am contents follow:

#templatesubdir = CardsAndLabels
#
#templates_DATA = .directory BusinessCards10.desktop LabelsL16.desktop
#templatesdir = $(kde_datadir)/kword/templates/$(templatesubdir)
#
#templatesrc_DATA = BusinessCards10.kwt LabelsL16.kwt
#templatesrcdir = $(kde_datadir)/kword/templates/$(templatesubdir)/.source
#
#kworddir = $(kde_datadir)/kword/icons
#kword_ICON = AUTO
#
