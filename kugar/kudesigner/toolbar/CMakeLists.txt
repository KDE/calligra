kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kudesigner/icons FILES  )

kde4_footer()



#original Makefile.am contents follow:

#kudesignericondir = $(kde_datadir)/kudesigner/icons
#kudesignericon_ICON = AUTO
