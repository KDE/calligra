




########### install files ###############

install( FILES  kpresenter_magicpoint_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( mgp2kpr.py  DESTINATION FILES)





#original Makefile.am contents follow:

#bin_SCRIPTS = mgp2kpr.py
#
#service_DATA = kpresenter_magicpoint_import.desktop
#
#servicedir = $(kde_servicesdir)
