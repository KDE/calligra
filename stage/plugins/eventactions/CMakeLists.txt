include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part)

set(calligrastageeventactions_PART_SRCS
    sound/KPrSoundEventAction.cpp
    sound/KPrSoundEventActionFactory.cpp
    sound/KPrSoundEventActionWidget.cpp
    Plugin.cpp
)

add_library(calligrastageeventactions MODULE ${calligrastageeventactions_PART_SRCS})


target_link_libraries(
    calligrastageeventactions
    calligrastageprivate
    flake
    KF6::KIOCore
    Phonon::phonon4qt6
    Qt6::Gui
)

install(TARGETS calligrastageeventactions  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/presentationeventactions)
