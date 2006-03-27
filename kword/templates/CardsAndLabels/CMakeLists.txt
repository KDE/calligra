kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files(  FILES  BusinessCards10.kwt LabelsL16.kwt )
install_files(  FILES  .directory BusinessCards10.desktop LabelsL16.desktop )
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
