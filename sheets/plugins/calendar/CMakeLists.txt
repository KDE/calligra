add_definitions(-DTRANSLATION_DOMAIN=\"calligrasheets_calendar\")

include_directories(${CMAKE_SOURCE_DIR}/sheets ${KOMAIN_INCLUDES})

set(kspread_plugin_tool_calendar_SRCS CalendarTool.cpp CalendarToolFactory.cpp CalendarToolWidget.cpp)
ki18n_wrap_ui(kspread_plugin_tool_calendar_SRCS CalendarToolWidget.ui)
add_library(kspread_plugin_tool_calendar MODULE ${kspread_plugin_tool_calendar_SRCS})
calligrasheets_plugin_desktop_to_json(kspread_plugin_tool_calendar kspread_plugin_tool_calendar.desktop)

target_link_libraries(kspread_plugin_tool_calendar calligrasheetscommon)

# TODO: calendar tool does not fit into UI anymore, needs rework. Thus not installing for now
#install(TARGETS kspread_plugin_tool_calendar DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/tools)
