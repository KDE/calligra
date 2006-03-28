kde4_header()

add_subdirectory( Computer )
add_subdirectory( Miscellaneous )




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/stencils/Hardware FILES  desc icon.xpm )

kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = Computer Miscellaneous
#
#hardwaredir = $(kde_datadir)/kivio/stencils/Hardware
#
#hardware_DATA = desc icon.xpm
