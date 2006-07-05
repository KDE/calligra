




########### install files ###############

install( FILES  simple.kgr allcolors.kgr  DESTINATION ${DATA_INSTALL_DIR}/karbon/gradients)
install( FILES  karbon_module.desktop  DESTINATION ${SERVICETYPES_INSTALL_DIR})
install( FILES  karbonpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  karbon.rc karbon_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon)
install( FILES  karbon.desktop  DESTINATION ${XDG_APPS_DIR})





#original Makefile.am contents follow:

#predefgradients_DATA = simple.kgr allcolors.kgr
#predefgradientsdir = $(kde_datadir)/karbon/gradients
#
#kdelnk_DATA = karbon.desktop
#kdelnkdir = $(kde_appsdir)/Office
#
#rc_DATA = karbon.rc karbon_readonly.rc
#rcdir = $(kde_datadir)/karbon
#kde_services_DATA = karbonpart.desktop
#kde_servicetypes_DATA = karbon_module.desktop
