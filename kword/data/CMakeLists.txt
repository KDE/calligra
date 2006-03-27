kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kword FILES  framestyles.xml tablestyles.xml tabletemplates.xml )

kde4_footer()



#original Makefile.am contents follow:

#kworddir = $(kde_datadir)/kword
#kword_DATA = framestyles.xml tablestyles.xml tabletemplates.xml
