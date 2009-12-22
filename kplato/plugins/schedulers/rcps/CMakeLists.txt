
include_directories( ${LIBRCPS_INCLUDE_DIR} ${KOSTORE_INCLUDES} ${CMAKE_SOURCE_DIR}/kplato/libs/kernel ${KDE4_INCLUDES} )

add_subdirectory( tests )

SET ( RCPSScheduler_SRCS
    KPlatoRCPSPlugin.cpp
    KPlatoRCPSScheduler.cpp
)

kde4_add_plugin( kplatorcpsscheduler ${RCPSScheduler_SRCS} )

target_link_libraries(
    kplatorcpsscheduler
    kplatokernel
    kostore
    ${LIBRCPS_LIBRARIES}
    )

install( TARGETS kplatorcpsscheduler DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES kplatorcpsscheduler.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
