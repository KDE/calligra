kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kword/expression FILES  expression.xml )

kde4_footer()



#original Makefile.am contents follow:

#
#scripts_DATA = expression.xml
#scriptsdir = $(kde_datadir)/kword/expression
#
