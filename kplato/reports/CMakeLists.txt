




########### install files ###############

install( FILES  resourcelist.ktf tasklist.ktf  DESTINATION ${DATA_INSTALL_DIR}/kplato/reports/.source)
install( FILES  resourcelist.desktop tasklist.desktop  DESTINATION ${DATA_INSTALL_DIR}/kplato/reports)





#original Makefile.am contents follow:

#kplatoreportdir = $(kde_datadir)/kplato/reports
#kplatoreport_DATA = resourcelist.desktop tasklist.desktop
#
#kplatoreportsourcedir = $(kde_datadir)/kplato/reports/.source
#kplatoreportsource_DATA = resourcelist.ktf tasklist.ktf
#
#EXTRA_DIST = $(kplatoreport_DATA)
