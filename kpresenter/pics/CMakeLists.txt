

add_subdirectory( rotate )




########### install files ###############

install( FILES  webslideshow-sidebar.png  DESTINATION ${DATA_INSTALL_DIR}/kpresenter/pics)

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )





#original Makefile.am contents follow:

#pics_DATA = webslideshow-sidebar.png
#picsdir = $(kde_datadir)/kpresenter/pics
#
#KDE_ICON = kpresenter
#
#SUBDIRS = rotate
