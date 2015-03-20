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

target_link_libraries(
    calligrastageeventactions
    calligrastageprivate
    flake
    KF5::KIOCore
    Phonon::phonon4qt5
    Qt5::Gui
    )

install(TARGETS calligrastageeventactions  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES calligrastageeventactions.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra )
