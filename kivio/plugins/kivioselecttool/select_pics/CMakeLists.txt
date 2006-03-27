kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/pics FILES  kivio_arrow.xpm )

kde4_footer()



#original Makefile.am contents follow:

##1########################################################!@#$
#####   Makefile.am generated automatically by KDEStudio   ####
##### WARNING! All changes made in this part will be lost! ####
###############################################################
#
#kiviopicsdir = $(kde_datadir)/kivio/pics
#
#kiviopics_DATA = kivio_arrow.xpm
#EXTRA_DIST = \
#		kivio_arrow.xpm
###############################################################
#####                  END KDEStudio part                  ####
##2########################################################!@#$
