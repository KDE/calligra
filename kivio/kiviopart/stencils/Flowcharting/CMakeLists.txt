kde4_header()

add_subdirectory( BasicFlowcharting )
add_subdirectory( Logic )
add_subdirectory( Extended )




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/stencils/Flowcharting FILES  desc icon.xpm )

kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = BasicFlowcharting Logic Extended
#
#flowchartingdir = $(kde_datadir)/kivio/stencils/Flowcharting
#
#flowcharting_DATA = desc icon.xpm
