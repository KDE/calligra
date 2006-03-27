kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kchart/icons FILES  )

kde4_footer()



#original Makefile.am contents follow:

#kcharticondir = $(kde_datadir)/kchart/icons
#kcharticon_ICON = AUTO
#
