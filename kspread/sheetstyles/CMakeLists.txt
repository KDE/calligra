kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kspread/sheetstyles FILES  standard1.ksts standard1.png standard1.xml standard2.ksts standard2.png standard2.xml )

kde4_footer()



#original Makefile.am contents follow:

#
#scripts_DATA = standard1.ksts standard1.png standard1.xml standard2.ksts standard2.png standard2.xml
#scriptsdir = $(kde_datadir)/kspread/sheetstyles
#
#
