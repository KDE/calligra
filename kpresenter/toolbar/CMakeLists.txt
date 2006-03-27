kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kpresenter/icons FILES  )

kde4_footer()



#original Makefile.am contents follow:

#kpresentericondir = $(kde_datadir)/kpresenter/icons
#kpresentericon_ICON = AUTO
