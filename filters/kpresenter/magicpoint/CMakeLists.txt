kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kpresenter_magicpoint_import.desktop )
install_files(  FILES  mgp2kpr.py )

kde4_footer()



#original Makefile.am contents follow:

#bin_SCRIPTS = mgp2kpr.py
#
#service_DATA = kpresenter_magicpoint_import.desktop
#
#servicedir = $(kde_servicesdir)
