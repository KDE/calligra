




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kspread/icons FILES  )
install_files( ${DATA_INSTALL_DIR}/kspread/templates/General/.source FILES  Worksheet.kst StudentIDCard.kst )
install_files( ${DATA_INSTALL_DIR}/kspread/templates/General FILES  .directory Worksheet.desktop StudentIDCard.desktop )





#original Makefile.am contents follow:

#template_DATA = .directory Worksheet.desktop StudentIDCard.desktop
#templatedir = $(kde_datadir)/kspread/templates/General
#
#templatesrc_DATA = Worksheet.kst StudentIDCard.kst
#templatesrcdir = $(kde_datadir)/kspread/templates/General/.source
#
#kspreadicondir = $(kde_datadir)/kspread/icons
#kspreadicon_ICON = AUTO
#
