




########### install files ###############

install( FILES  empty.kft basicflow.kft  DESTINATION ${DATA_INSTALL_DIR}/kivio/templates/Basic/.source)
install( FILES  .directory empty.desktop basicflow.desktop  DESTINATION ${DATA_INSTALL_DIR}/kivio/templates/Basic)
install( FILES   DESTINATION ${DATA_INSTALL_DIR}/kivio/icons)





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
