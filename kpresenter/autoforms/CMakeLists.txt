

add_subdirectory( Arrows )
add_subdirectory( Connections )




########### install files ###############

install( FILES  .autoforms  DESTINATION ${DATA_INSTALL_DIR}/kpresenter/autoforms)





#original Makefile.am contents follow:

#
#SUBDIRS = Arrows Connections
#
#autoforms_DATA = .autoforms
#autoformsdir = $(kde_datadir)/kpresenter/autoforms
#
