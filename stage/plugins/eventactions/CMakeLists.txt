include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part)
if(NOT QT_MAC_USE_COCOA)
    include_directories(${PHONON_INCLUDE_DIR}/phonon)
else(NOT QT_MAC_USE_COCOA)
    include_directories(${PHONON_INCLUDE_DIR})
endif(NOT QT_MAC_USE_COCOA)

set(calligrastageeventactions_PART_SRCS
    sound/KPrSoundEventAction.cpp
    sound/KPrSoundEventActionFactory.cpp
    sound/KPrSoundEventActionWidget.cpp
    Plugin.cpp
)

kde4_add_plugin(calligrastageeventactions ${calligrastageeventactions_PART_SRCS})

target_link_libraries(
    calligrastageeventactions
    calligrastageprivate
    flake
    ${KDE4_KIO_LIBRARY}
    ${KDE4_PHONON_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    )

install(TARGETS calligrastageeventactions  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES calligrastageeventactions.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
