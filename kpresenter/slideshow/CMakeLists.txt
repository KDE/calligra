kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kpresenter/slideshow FILES  home.png first.png prev.png next.png last.png  )

kde4_footer()



#original Makefile.am contents follow:

#
#slideshow_DATA = home.png first.png prev.png next.png last.png 
#
#slideshowdir = $(kde_datadir)/kpresenter/slideshow
#
