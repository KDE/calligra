

include_directories( ${KOFFICEUI_INCLUDES} ${KDE4_INCLUDES}  )


########### next target ###############

set(kspreadinsertcalendar_PART_SRCS InsertCalendar.cpp InsertCalendarDialog.cpp )

kde4_automoc(${kspreadinsertcalendar_PART_SRCS})

kde4_add_ui_files(kspreadinsertcalendar_PART_SRCS InsertCalendarDialog.ui )

kde4_add_plugin(kspreadinsertcalendar WITH_PREFIX ${kspreadinsertcalendar_PART_SRCS})



target_link_libraries(kspreadinsertcalendar  ${KDE4_KDECORE_LIBS} kspreadcommon )

install(TARGETS kspreadinsertcalendar  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  insertcalendar.rc  DESTINATION ${DATA_INSTALL_DIR}/kspread/kpartplugins)

