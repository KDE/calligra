kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kexi/scripts/exportxhtml FILES  ExportXHTML.py ExportXHTML.rc )

kde4_footer()



#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#scriptsdir = $(kde_datadir)/kexi/scripts/exportxhtml
#scripts_SCRIPTS = ExportXHTML.py ExportXHTML.rc
