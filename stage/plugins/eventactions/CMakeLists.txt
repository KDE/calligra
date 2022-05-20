include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part)

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
    Phonon::phonon4qt5
    Qt5::Gui
)

install(TARGETS calligrastageeventactions  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/presentationeventactions)
