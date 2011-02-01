
include_directories(
    ${CMAKE_BINARY_DIR}/kplato/plugins/filters/icalendar/export
    ${CMAKE_SOURCE_DIR}/kplato
    ${CMAKE_SOURCE_DIR}/kplato/libs/kernel
    ${KOMAIN_INCLUDES}
)

set(icalendarexport_PART_SRCS 
   icalendarexport.cpp
)


kde4_add_plugin(planicalexport ${icalendarexport_PART_SRCS})

target_link_libraries(planicalexport kplatoprivate kplatokernel komain ${LIBKCAL_LIBRARY})

install(TARGETS planicalexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  plan_icalendar_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

