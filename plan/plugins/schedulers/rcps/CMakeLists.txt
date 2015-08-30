add_definitions(-DTRANSLATION_DOMAIN=\"planrcpsplugin\")

set(LIBRCPS_INCLUDE_DIR 3rdparty/LibRCPS/src)
include_directories( ${LIBRCPS_INCLUDE_DIR} ${KOODF_INCLUDES} ${CMAKE_SOURCE_DIR}/plan/libs/kernel  )

add_subdirectory( 3rdparty )
add_subdirectory( tests )

set ( RCPSScheduler_SRCS
    KPlatoRCPSPlugin.cpp
    KPlatoRCPSScheduler.cpp
)

add_library(kplatorcpsscheduler MODULE ${RCPSScheduler_SRCS} )
kcoreaddons_desktop_to_json(kplatorcpsscheduler planrcpsscheduler.desktop)

target_link_libraries(
    kplatorcpsscheduler
    kplatokernel
    rcps_plan
    ${KDE4_KDECORE_LIBS}
#    ${LIBRCPS_LIBRARIES}
)

install( TARGETS kplatorcpsscheduler DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR} )
