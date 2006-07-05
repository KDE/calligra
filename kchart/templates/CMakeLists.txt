




########### install files ###############

install( FILES   DESTINATION ${DATA_INSTALL_DIR}/kchart/icons)
install( FILES  BarChart.chrt Empty.chrt  DESTINATION ${DATA_INSTALL_DIR}/kchart/templates/chart/.source)
install( FILES  .directory BarChart.desktop Empty.desktop  DESTINATION ${DATA_INSTALL_DIR}/kchart/templates/chart)





#original Makefile.am contents follow:

#template_DATA = .directory BarChart.desktop Empty.desktop
#templatedir = $(kde_datadir)/kchart/templates/chart
#
#templatesrc_DATA = BarChart.chrt Empty.chrt
#templatesrcdir = $(kde_datadir)/kchart/templates/chart/.source
#
#kcharticondir = $(kde_datadir)/kchart/icons
#kcharticon_ICON = AUTO
#
