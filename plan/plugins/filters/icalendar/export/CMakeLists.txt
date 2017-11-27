
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PLAN_SOURCE_DIR}
    ${PLANKERNEL_INCLUDES}
    ${PLANMAIN_INCLUDES}
)

set(icalendarexport_PART_SRCS 
   icalendarexport.cpp
)


add_library(planicalexport MODULE ${icalendarexport_PART_SRCS})
#calligraplan_filter_desktop_to_json(planicalexport plan_icalendar_export.desktop)
if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(planicalexport plan_icalendar_export.desktop)
else()
    kcoreaddons_desktop_to_json(planicalexport plan_icalendar_export.desktop
#         SERVICE_TYPES ${PLAN_SOURCE_DIR}/servicetypes/calligraplan_filter.desktop
    )
endif()

target_link_libraries(planicalexport planprivate kplatokernel planmain KF5::CalendarCore)

install(TARGETS planicalexport  DESTINATION ${PLUGIN_INSTALL_DIR}/calligraplan/formatfilters)
