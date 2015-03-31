
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

target_link_libraries(planicalexport planprivate kplatokernel komain KF5::CalendarCore)

install(TARGETS planicalexport  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  plan_icalendar_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)

