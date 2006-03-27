kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/icons FILES  )

kde4_footer()



#original Makefile.am contents follow:

#kivioicondir = $(kde_datadir)/kivio/icons
#kivioicon_ICON = AUTO
