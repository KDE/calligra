
include_directories( ${CMAKE_SOURCE_DIR}/kplato/libs/kernel ${KDE4_INCLUDES} )

SET ( RCPSScheduler_SRCS
    KPlatoRCPSPlugin.cpp
)

kde4_add_plugin( kplatorcpsscheduler ${RCPSScheduler_SRCS} )

target_link_libraries(
    kplatorcpsscheduler
    kplatokernel
    ${KDE4_KDECORE_LIBS}
    ${QT_QTCORE_LIBRARY}
    )

install( TARGETS kplatorcpsscheduler DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES kplatorcpsscheduler.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
