




########### install files ###############

install( FILES  CopyCenter.py CopyCenterPluginQtSQL.py CopyCenterPluginKexiDB.py CopyCenter.rc readme.html  DESTINATION ${DATA_INSTALL_DIR}/kexi/scripts/copycenter)





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#scriptsdir = $(kde_datadir)/kexi/scripts/copycenter
#scripts_SCRIPTS = CopyCenter.py CopyCenterPluginQtSQL.py CopyCenterPluginKexiDB.py CopyCenter.rc readme.html
