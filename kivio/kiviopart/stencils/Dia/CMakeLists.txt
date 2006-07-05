

add_subdirectory( Assorted )
add_subdirectory( Circuit )
add_subdirectory( Cisco )
add_subdirectory( Civil )
add_subdirectory( Contact )
add_subdirectory( Electric )
add_subdirectory( Jigsaw )
add_subdirectory( MSE )
add_subdirectory( Network )
add_subdirectory( Pneumatic )
add_subdirectory( SDL )
add_subdirectory( Sybase )




########### install files ###############

install( FILES  desc icon.xpm  DESTINATION ${DATA_INSTALL_DIR}/kivio/stencils/Dia)





#original Makefile.am contents follow:

#SUBDIRS = Assorted Circuit Cisco Civil Contact Electric Jigsaw\
#	MSE Network Pneumatic SDL Sybase
#
#diashapesdir = $(kde_datadir)/kivio/stencils/Dia
#
#diashapes_DATA = desc icon.xpm
#EXTRA_DIST = desc icon.xpm
