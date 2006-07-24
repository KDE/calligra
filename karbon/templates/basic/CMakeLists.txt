
install( FILES  empty.karbon  DESTINATION ${DATA_INSTALL_DIR}/karbon/templates/Basic/.source)
install( FILES  .directory empty.desktop  DESTINATION ${DATA_INSTALL_DIR}/karbon/templates/Basic)
kde4_install_icons( ${DATA_INSTALL_DIR}/karbon/icons crystalsvg )




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
