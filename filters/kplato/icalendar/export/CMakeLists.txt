
include_directories(
    ${CMAKE_BINARY_DIR}/filters/kplato/icalendar/export
)

set(icalendarexport_PART_SRCS 
   icalendarexport.cpp
)


kde4_add_plugin(icalendarexport WITH_PREFIX ${icalendarexport_PART_SRCS})

target_link_libraries(icalendarexport kplatoprivate kplatokernel komain ${LIBKCAL_LIBRARY})

install(TARGETS icalendarexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kplato_icalendar_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

