include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part)
if(NOT QT_MAC_USE_COCOA)
    include_directories(${PHONON_INCLUDE_DIR}/phonon)
else()
    include_directories(${PHONON_INCLUDE_DIR})
endif()

set(calligrastageeventactions_PART_SRCS
    sound/KPrSoundEventAction.cpp
    sound/KPrSoundEventActionFactory.cpp
    sound/KPrSoundEventActionWidget.cpp
    Plugin.cpp
)

add_library(calligrastageeventactions MODULE ${calligrastageeventactions_PART_SRCS})

calligra_presentationeventaction_desktop_to_json(calligrastageeventactions calligrastageeventactions.desktop)

target_link_libraries(
    calligrastageeventactions
    calligrastageprivate
    flake
    KF5::KIOCore
    ${PHONON_LIBS}
    Qt5::Gui
    )

install(TARGETS calligrastageeventactions  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/presentationeventactions)
