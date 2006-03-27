kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/stencils/Hardware/Miscellaneous FILES  cd-rom.sml  cd-rom.xpm  CellPhone.sml  CellPhone.xpm  key.sml  key.xpm  desc  icon.xpm )

kde4_footer()



#original Makefile.am contents follow:

#miscdir = $(kde_datadir)/kivio/stencils/Hardware/Miscellaneous
#
#misc_DATA = \
#cd-rom.sml \
#cd-rom.xpm \
#CellPhone.sml \
#CellPhone.xpm \
#key.sml \
#key.xpm \
#desc \
#icon.xpm