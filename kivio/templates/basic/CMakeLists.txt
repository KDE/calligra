




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/templates/Basic/.source FILES  empty.kft basicflow.kft )
install_files( ${DATA_INSTALL_DIR}/kivio/templates/Basic FILES  .directory empty.desktop basicflow.desktop )
install_files( ${DATA_INSTALL_DIR}/kivio/icons FILES  )





#original Makefile.am contents follow:

#templates_DATA = .directory empty.desktop basicflow.desktop
#templatesdir = $(kde_datadir)/kivio/templates/Basic
#
#templatesrc_DATA = empty.kft basicflow.kft
#templatesrcdir = $(kde_datadir)/kivio/templates/Basic/.source
#
#kivioicondir = $(kde_datadir)/kivio/icons
#kivioicon_ICON = AUTO
#
