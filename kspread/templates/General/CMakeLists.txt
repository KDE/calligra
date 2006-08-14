




########### install files ###############

install( FILES  Worksheet.kst StudentIDCard.kst  DESTINATION ${DATA_INSTALL_DIR}/kspread/templates/General/.source)
install( FILES  .directory Worksheet.desktop StudentIDCard.desktop  DESTINATION ${DATA_INSTALL_DIR}/kspread/templates/General)

kde4_install_icons( ${DATA_INSTALL_DIR}/kspread/icons  )



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
