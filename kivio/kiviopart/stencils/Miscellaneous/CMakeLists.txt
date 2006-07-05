

add_subdirectory( People )
add_subdirectory( Buildings )
add_subdirectory( Transport )
add_subdirectory( Arrows )
add_subdirectory( NassiShneiderman )
add_subdirectory( ER )




########### install files ###############

install( FILES  desc icon.xpm  DESTINATION ${DATA_INSTALL_DIR}/kivio/stencils/Miscellaneous)





#original Makefile.am contents follow:

#SUBDIRS = People Buildings Transport Arrows NassiShneiderman ER
#
#miscdir = $(kde_datadir)/kivio/stencils/Miscellaneous
#
#misc_DATA = desc icon.xpm
