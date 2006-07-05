

add_subdirectory( BasicFlowcharting )
add_subdirectory( Logic )
add_subdirectory( Extended )




########### install files ###############

install( FILES  desc icon.xpm  DESTINATION ${DATA_INSTALL_DIR}/kivio/stencils/Flowcharting)





#original Makefile.am contents follow:

#SUBDIRS = BasicFlowcharting Logic Extended
#
#flowchartingdir = $(kde_datadir)/kivio/stencils/Flowcharting
#
#flowcharting_DATA = desc icon.xpm
