

add_subdirectory( Maps )
add_subdirectory( Flags )




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/stencils/Geographic FILES  desc icon.xpm )





#original Makefile.am contents follow:

#SUBDIRS = Maps Flags
#
#geographicdir = $(kde_datadir)/kivio/stencils/Geographic
#
#geographic_DATA = desc icon.xpm
