




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/karbon/icons FILES  )
install_files( ${DATA_INSTALL_DIR}/karbon/templates/Basic/.source FILES  empty.karbon )
install_files( ${DATA_INSTALL_DIR}/karbon/templates/Basic FILES  .directory empty.desktop )





#original Makefile.am contents follow:

#templates_DATA = .directory empty.desktop
#templatesdir = $(kde_datadir)/karbon/templates/Basic
#
#templatesrc_DATA = empty.karbon
#templatesrcdir = $(kde_datadir)/karbon/templates/Basic/.source
#
#karbonicondir = $(kde_datadir)/karbon/icons
#karbonicon_ICON = AUTO
#
