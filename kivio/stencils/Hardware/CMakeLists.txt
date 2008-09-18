

add_subdirectory( Computer )
add_subdirectory( Miscellaneous )




########### install files ###############

install( FILES  desc icon.xpm  DESTINATION ${DATA_INSTALL_DIR}/kivio/stencils/Hardware)





#original Makefile.am contents follow:

#SUBDIRS = Computer Miscellaneous
#
#hardwaredir = $(kde_datadir)/kivio/stencils/Hardware
#
#hardware_DATA = desc icon.xpm
