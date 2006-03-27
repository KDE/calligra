kde4_header()

add_subdirectory( Maps ) 
add_subdirectory( Flags ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/stencils/Geographic FILES  desc icon.xpm )

kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = Maps Flags
#
#geographicdir = $(kde_datadir)/kivio/stencils/Geographic
#
#geographic_DATA = desc icon.xpm
