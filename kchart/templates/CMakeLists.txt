




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kchart/icons FILES  )
install_files( ${DATA_INSTALL_DIR}/kchart/templates/chart/.source FILES  BarChart.chrt Empty.chrt )
install_files( ${DATA_INSTALL_DIR}/kchart/templates/chart FILES  .directory BarChart.desktop Empty.desktop )





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
