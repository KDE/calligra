kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/stencils/Miscellaneous/Transport FILES  desc  icon.xpm  Truck.sml  Truck.xpm )

kde4_footer()



#original Makefile.am contents follow:

#transportdir = $(kde_datadir)/kivio/stencils/Miscellaneous/Transport
#
#transport_DATA = \
#desc \
#icon.xpm \
#Truck.sml \
#Truck.xpm
