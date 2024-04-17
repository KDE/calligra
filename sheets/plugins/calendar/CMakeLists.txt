add_definitions(-DTRANSLATION_DOMAIN=\"calligrasheets_calendar\")

set(kspread_plugin_tool_calendar_SRCS
    CalendarTool.cpp
    CalendarToolFactory.cpp
    CalendarToolWidget.cpp
)

ki18n_wrap_ui(kspread_plugin_tool_calendar_SRCS CalendarToolWidget.ui)

add_library(kspread_plugin_tool_calendar MODULE ${kspread_plugin_tool_calendar_SRCS})

target_include_directories( kspread_plugin_tool_calendar
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
    ${CMAKE_CURRENT_BINARY_DIR}/../../
)


target_link_libraries(kspread_plugin_tool_calendar
    PUBLIC
    Qt6::Core
    calligrasheetspartlib
)

# TODO: calendar tool does not fit into UI anymore, needs rework. Thus not installing for now
#install(TARGETS kspread_plugin_tool_calendar DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/tools)
