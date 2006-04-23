kde4_header()




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kword/templates/CardsAndLabels/  FILES  BusinessCards10.kwt LabelsL16.kwt )
install_files( ${DATA_INSTALL_DIR}/kword/templates/CardsAndLabels FILES  .directory BusinessCards10.desktop LabelsL16.desktop )
install_files( ${DATA_INSTALL_DIR}/kword/icons FILES  )

kde4_footer()



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
