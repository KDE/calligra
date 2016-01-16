
include_directories(
    ${CMAKE_BINARY_DIR}/plan/plugins/filters/icalendar/export
    ${CMAKE_SOURCE_DIR}/plan
    ${CMAKE_SOURCE_DIR}/plan/libs/kernel
    ${KOMAIN_INCLUDES}
)

set(icalendarexport_PART_SRCS 
   icalendarexport.cpp
)


add_library(planicalexport MODULE ${icalendarexport_PART_SRCS})
kcoreaddons_desktop_to_json(planicalexport plan_icalendar_export.desktop)

target_link_libraries(planicalexport planprivate kplatokernel komain KF5::CalendarCore)

install(TARGETS planicalexport  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)
