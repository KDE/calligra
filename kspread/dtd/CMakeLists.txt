kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kspread/dtd FILES  kspread.dtd )

kde4_footer()



#original Makefile.am contents follow:

#dtd_DATA = kspread.dtd
#
#dtddir = $(kde_datadir)/kspread/dtd
#
