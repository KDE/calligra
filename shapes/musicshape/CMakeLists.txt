add_subdirectory( fonts )
add_subdirectory( core )

include_directories( ${KOGUIUTILS_INCLUDES} )

########### Flake Plugin library ###############

set( musiccore_SRCS
    core/Sheet.cpp
    core/Part.cpp
    core/PartGroup.cpp
    core/Staff.cpp
    core/Voice.cpp
    core/Bar.cpp
    core/VoiceBar.cpp
    core/MusicElement.cpp
    core/Chord.cpp
    core/Note.cpp
    core/Clef.cpp
    core/KeySignature.cpp
    core/TimeSignature.cpp
)

SET (musicshape_LIB_SRCS
    MusicShape.cpp
    MusicShapeFactory.cpp
    MusicTool.cpp
    MusicToolFactory.cpp
    MusicStyle.cpp
    Engraver.cpp
    Renderer.cpp

    dialogs/PartsWidget.cpp

    ${musiccore_SRCS}
)

kde4_add_ui_files(musicshape_LIB_SRCS
    dialogs/PartsWidget.ui
)

kde4_automoc(${musicshape_LIB_SRCS})
kde4_add_plugin(musicshape ${musicshape_LIB_SRCS})

target_link_libraries(musicshape flake koguiutils)
install(TARGETS musicshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES musicshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
