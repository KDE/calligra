kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${SERVICETYPES_INSTALL_DIR} FILES  kofilter.desktop koplugin.desktop  kofficepart.desktop kochart.desktop kofilterwrapper.desktop )

kde4_footer()



#original Makefile.am contents follow:

#
#servicetype_DATA = kofilter.desktop koplugin.desktop \
#kofficepart.desktop kochart.desktop kofilterwrapper.desktop
#servicetypedir = $(kde_servicetypesdir)
