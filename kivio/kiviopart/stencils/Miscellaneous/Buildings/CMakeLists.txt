kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/stencils/Miscellaneous/Buildings FILES  desc  Factory.sml  Factory.xpm  House.sml  House.xpm  icon.xpm  Office2.xpm  Office.sml  Office.xpm )

kde4_footer()



#original Makefile.am contents follow:

#buildingsdir = $(kde_datadir)/kivio/stencils/Miscellaneous/Buildings
#
#buildings_DATA = \
#desc \
#Factory.sml \
#Factory.xpm \
#House.sml \
#House.xpm \
#icon.xpm \
#Office2.xpm \
#Office.sml \
#Office.xpm
