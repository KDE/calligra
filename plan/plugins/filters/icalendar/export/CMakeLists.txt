
include_directories(
    ${CMAKE_BINARY_DIR}/plan/plugins/filters/icalendar/export
    ${CMAKE_SOURCE_DIR}/plan
    ${CMAKE_SOURCE_DIR}/plan/libs/kernel
    ${KOMAIN_INCLUDES}
    ${KDEPIMLIBS_INCLUDE_DIRS}
)

set(icalendarexport_PART_SRCS 
   icalendarexport.cpp
)


kde4_add_plugin(planicalexport ${icalendarexport_PART_SRCS})

target_link_libraries(planicalexport planprivate kplatokernel komain ${LIBKCAL_LIBRARY})

install(TARGETS planicalexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  plan_icalendar_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

