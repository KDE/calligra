kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kplato/icons FILES  )

kde4_footer()



#original Makefile.am contents follow:

#kplatoicondir = $(kde_datadir)/kplato/icons
#kplatoicon_ICON = AUTO
