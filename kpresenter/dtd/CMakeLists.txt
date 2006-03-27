kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kpresenter/dtd FILES  kpresenter.dtd )

kde4_footer()



#original Makefile.am contents follow:

#dtd_DATA = kpresenter.dtd
#
#dtddir = $(kde_datadir)/kpresenter/dtd
#
