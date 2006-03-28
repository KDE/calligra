kde4_header()

add_subdirectory( rotate )




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kpresenter/pics FILES  webslideshow-sidebar.png )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

#pics_DATA = webslideshow-sidebar.png
#picsdir = $(kde_datadir)/kpresenter/pics
#
#KDE_ICON = kpresenter
#
#SUBDIRS = rotate
