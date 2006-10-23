

include_directories( ${KOFFICEUI_INCLUDES} ${KDE4_INCLUDES}  )


########### next target ###############

set(kspreadinsertcalendar_PART_SRCS kspread_plugininsertcalendar.cc kspread_insertcalendardialog.cc )

kde4_automoc(${kspreadinsertcalendar_PART_SRCS})

kde4_add_ui_files(kspreadinsertcalendar_PART_SRCS kspread_insertcalendardialogbase.ui )

kde4_add_plugin(kspreadinsertcalendar WITH_PREFIX ${kspreadinsertcalendar_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kspreadinsertcalendar )

target_link_libraries(kspreadinsertcalendar  ${KDE4_KDECORE_LIBS} kspreadcommon )

install(TARGETS kspreadinsertcalendar  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  insertcalendar.rc  DESTINATION ${DATA_INSTALL_DIR}/kspread/kpartplugins)

