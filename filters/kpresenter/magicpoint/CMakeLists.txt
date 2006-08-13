




########### install files ###############

install( FILES  kpresenter_magicpoint_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( PROGRAMS mgp2kpr.py  DESTINATION ${BIN_INSTALL_DIR})





#original Makefile.am contents follow:

#bin_SCRIPTS = mgp2kpr.py
#
#service_DATA = kpresenter_magicpoint_import.desktop
#
#servicedir = $(kde_servicesdir)
