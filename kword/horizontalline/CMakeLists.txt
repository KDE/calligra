kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kword/horizontalline FILES  line.png )

kde4_footer()



#original Makefile.am contents follow:

#
#img_DATA = line.png
#imgdir = $(kde_datadir)/kword/horizontalline
#
