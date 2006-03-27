kde4_header()




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/stencils/Dia/Sybase FILES  $(SHAPES) 	desc icon.xpm )

kde4_footer()



#original Makefile.am contents follow:

#sybasedir = $(kde_datadir)/kivio/stencils/Dia/Sybase
#
#SHAPES = \
#	client.shape \
#	client.xpm \
#	dataserver.shape \
#	dataserver.xpm \
#	ltm.shape \
#	ltm.xpm \
#	repserver.shape \
#	repserver.xpm \
#	rsm.shape \
#	rsm.xpm \
#	stableq.shape \
#	stableq.xpm
#
#EXTRA_DIST = $(SHAPES)\
#	desc icon.xpm
#
#sybase_DATA = $(SHAPES)\
#	desc icon.xpm
