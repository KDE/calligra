project( spacenavigator )

include_directories( ${FLAKE_INCLUDES}  ${SPNAV_INCLUDE_DIR})


########### Flake Plugin library ###############

set ( spacenavigator_SRCS
    SpaceNavigatorDebug.cpp
    Plugin.cpp
    SpaceNavigatorDevice.cpp
    SpaceNavigatorPollingThread.cpp
    SpaceNavigatorEvent.cpp
)

add_library(calligra_device_spacenavigator MODULE ${spacenavigator_SRCS})
target_link_libraries(calligra_device_spacenavigator flake ${SPNAV_LIBRARY} ${X11_LIBRARIES})
install(TARGETS calligra_device_spacenavigator DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/devices)
