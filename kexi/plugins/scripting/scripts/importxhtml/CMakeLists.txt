kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kexi/scripts/importxhtml FILES  ImportXHTML.py ImportXHTML.rc )

kde4_footer()



#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#scriptsdir = $(kde_datadir)/kexi/scripts/importxhtml
#scripts_SCRIPTS = ImportXHTML.py ImportXHTML.rc
