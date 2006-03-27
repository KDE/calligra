kde4_header()




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kexi/scripts/projectdocumentor FILES  ProjectDocumentor.py ProjectDocumentor.rc )

kde4_footer()



#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#scriptsdir = $(kde_datadir)/kexi/scripts/projectdocumentor
#scripts_SCRIPTS = ProjectDocumentor.py ProjectDocumentor.rc
