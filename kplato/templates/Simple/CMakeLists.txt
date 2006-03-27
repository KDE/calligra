kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kplato/icons FILES  )
install_files( ${DATA_INSTALL_DIR}/kplato/templates/Simple/.source FILES  Plain.kplatot 8HourDay-40HourWeek.kplatot )
install_files( ${DATA_INSTALL_DIR}/kplato/templates/Simple FILES  .directory Plain.desktop 8HourDay-40HourWeek.desktop )

kde4_footer()



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
