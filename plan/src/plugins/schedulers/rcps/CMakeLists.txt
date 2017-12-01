add_definitions(-DTRANSLATION_DOMAIN=\"calligraplan_scheduler_rcps\")

set(LIBRCPS_INCLUDE_DIR 3rdparty/LibRCPS/src)
include_directories( ${LIBRCPS_INCLUDE_DIR} ${PLANODF_INCLUDES} ${PLAN_SOURCE_DIR}/libs/kernel  )

add_subdirectory( 3rdparty )
if(BUILD_TESTING)
    add_subdirectory( tests )
endif()

set ( RCPSScheduler_SRCS
    KPlatoRCPSPlugin.cpp
    KPlatoRCPSScheduler.cpp
)

add_library(kplatorcpsscheduler MODULE ${RCPSScheduler_SRCS} )
#calligraplan_scheduler_desktop_to_json(kplatorcpsscheduler planrcpsscheduler.desktop)
if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(kplatorcpsscheduler planrcpsscheduler.desktop)
else()
    kcoreaddons_desktop_to_json(kplatorcpsscheduler planrcpsscheduler.desktop
        SERVICE_TYPES ${PLAN_SOURCE_DIR}/libs/kernel/plan_schedulerplugin.desktop
    )
endif()

target_link_libraries(
    kplatorcpsscheduler
    kplatokernel
    rcps_plan
#    ${LIBRCPS_LIBRARIES}
)

install( TARGETS kplatorcpsscheduler DESTINATION ${PLUGIN_INSTALL_DIR}/calligraplan/schedulers )
