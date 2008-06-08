include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${KOGUIUTILS_INCLUDES} ${CMAKE_SOURCE_DIR}/kpresenter/part )

set(kpresentereventactions_PART_SRCS
    sound/KPrSoundEventAction.cpp
    sound/KPrSoundEventActionFactory.cpp
    Plugin.cpp
)

kde4_add_plugin(kpresentereventactions ${kpresentereventactions_PART_SRCS})

target_link_libraries(kpresentereventactions kpresenterprivate flake)

install(TARGETS kpresentereventactions  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kpresentereventactions.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
