include_directories(${CMAKE_SOURCE_DIR}/kspread ${KOGUIUTILS_INCLUDES})


########### next target ###############

set(kspreadinsertcalendar_PART_SRCS InsertCalendar.cpp InsertCalendarDialog.cpp )
kde4_add_ui_files(kspreadinsertcalendar_PART_SRCS InsertCalendarDialog.ui )
kde4_add_plugin(kspreadinsertcalendar WITH_PREFIX ${kspreadinsertcalendar_PART_SRCS})
target_link_libraries(kspreadinsertcalendar  ${KDE4_KDECORE_LIBS} kspreadcommon )
install(TARGETS kspreadinsertcalendar  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  insertcalendar.rc  DESTINATION ${DATA_INSTALL_DIR}/kspread/kpartplugins)


########### next target ###############
########### Calendar Tool ###############

set(kspread_plugin_tool_calendar_SRCS CalendarTool.cpp CalendarToolFactory.cpp)
kde4_add_ui_files(kspread_plugin_tool_calendar_SRCS CalendarToolWidget.ui)
kde4_add_plugin(kspread_plugin_tool_calendar ${kspread_plugin_tool_calendar_SRCS})
target_link_libraries(kspread_plugin_tool_calendar kspreadcommon)
install(TARGETS kspread_plugin_tool_calendar DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspread_plugin_tool_calendar.desktop DESTINATION ${SERVICES_INSTALL_DIR})
