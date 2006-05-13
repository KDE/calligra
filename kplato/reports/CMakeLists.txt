




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kplato/reports/.source FILES  resourcelist.ktf tasklist.ktf )
install_files( ${DATA_INSTALL_DIR}/kplato/reports FILES  resourcelist.desktop tasklist.desktop )





#original Makefile.am contents follow:

#kplatoreportdir = $(kde_datadir)/kplato/reports
#kplatoreport_DATA = resourcelist.desktop tasklist.desktop
#
#kplatoreportsourcedir = $(kde_datadir)/kplato/reports/.source
#kplatoreportsource_DATA = resourcelist.ktf tasklist.ktf
#
#EXTRA_DIST = $(kplatoreport_DATA)
