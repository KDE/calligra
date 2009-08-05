include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/kpresenter/part ${QT_PHONON_INCLUDE_DIR})

set(kpresentereventactions_PART_SRCS
    sound/KPrSoundEventAction.cpp
    sound/KPrSoundEventActionFactory.cpp
    sound/KPrSoundEventActionWidget.cpp
    Plugin.cpp
)

kde4_add_plugin(kpresentereventactions ${kpresentereventactions_PART_SRCS})

target_link_libraries(
    kpresentereventactions
    kpresenterprivate
    flake
    kostore
    ${KDE4_KIO_LIBRARY}
    ${KDE4_PHONON_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    )

install(TARGETS kpresentereventactions  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kpresentereventactions.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
