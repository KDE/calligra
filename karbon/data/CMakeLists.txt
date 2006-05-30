




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/karbon/gradients FILES  simple.kgr allcolors.kgr )
install_files( ${SERVICETYPES_INSTALL_DIR} FILES  karbon_module.desktop )
install_files( ${SERVICES_INSTALL_DIR} FILES  karbonpart.desktop )
install_files( ${DATA_INSTALL_DIR}/karbon FILES  karbon.rc karbon_readonly.rc )
install_files( ${XDG_APPS_DIR} FILES  karbon.desktop )





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
