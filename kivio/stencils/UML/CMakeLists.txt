

add_subdirectory( ActivityDiagrams )
add_subdirectory( ClassDiagrams )




########### install files ###############

install( FILES  desc icon.xpm  DESTINATION ${DATA_INSTALL_DIR}/kivio/stencils/UML)





#original Makefile.am contents follow:

#SUBDIRS = ActivityDiagrams ClassDiagrams
#umldir = $(kde_datadir)/kivio/stencils/UML
#
#uml_DATA = desc icon.xpm
#EXTRA_DIST = \
#		desc\
#		icon.xpm
