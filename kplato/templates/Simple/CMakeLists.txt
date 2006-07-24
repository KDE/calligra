




########### install files ###############

install( FILES  Plain.kplatot 8HourDay-40HourWeek.kplatot  DESTINATION ${DATA_INSTALL_DIR}/kplato/templates/Simple/.source)
install( FILES  .directory Plain.desktop 8HourDay-40HourWeek.desktop  DESTINATION ${DATA_INSTALL_DIR}/kplato/templates/Simple)

kde4_install_icons( ${DATA_INSTALL_DIR}/kplato/icons crystalsvg )



#original Makefile.am contents follow:

#template_DATA = .directory Plain.desktop 8HourDay-40HourWeek.desktop
#templatedir = $(kde_datadir)/kplato/templates/Simple
#
#templatesrc_DATA = Plain.kplatot 8HourDay-40HourWeek.kplatot
#templatesrcdir = $(kde_datadir)/kplato/templates/Simple/.source
#
#kplatoicondir = $(kde_datadir)/kplato/icons
#kplatoicon_ICON = AUTO
#
