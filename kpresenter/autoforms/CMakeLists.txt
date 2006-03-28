kde4_header()

add_subdirectory( Arrows )
add_subdirectory( Connections )




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kpresenter/autoforms FILES  .autoforms )

kde4_footer()



#original Makefile.am contents follow:

#
#SUBDIRS = Arrows Connections
#
#autoforms_DATA = .autoforms
#autoformsdir = $(kde_datadir)/kpresenter/autoforms
#
