




########### install files ###############

install( FILES  tabletemplates.dtd  DESTINATION ${DATA_INSTALL_DIR}/kword/dtd)
install( FILES  framestyles.dtd  DESTINATION ${DATA_INSTALL_DIR}/kword/dtd)
install( FILES  tablestyles.dtd  DESTINATION ${DATA_INSTALL_DIR}/kword/dtd)
install( FILES  kword.dtd  DESTINATION ${DATA_INSTALL_DIR}/kword/dtd)





#original Makefile.am contents follow:

#dtd_DATA = kword.dtd
#
#dtddir = $(kde_datadir)/kword/dtd
#
#fs_DATA = framestyles.dtd
#
#fsdir = $(kde_datadir)/kword/dtd
#
#ts_DATA = tablestyles.dtd
#
#tsdir = $(kde_datadir)/kword/dtd
#
#tt_DATA = tabletemplates.dtd
#
#ttdir = $(kde_datadir)/kword/dtd
#
