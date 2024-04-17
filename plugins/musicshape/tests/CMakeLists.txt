
set( musicwidgettest_SRCS
    MusicWidget.cpp
    MusicWidgetTest.cpp

    ../core/Bar.cpp
    ../core/Chord.cpp
    ../core/Clef.cpp
    ../core/Global.cpp
    ../core/KeySignature.cpp
    ../core/Note.cpp
    ../core/Part.cpp
    ../core/PartGroup.cpp
    ../core/Sheet.cpp
    ../core/Staff.cpp
    ../core/StaffElement.cpp
    ../core/StaffSystem.cpp
    ../core/TimeSignature.cpp
    ../core/Voice.cpp
    ../core/VoiceBar.cpp
    ../core/VoiceElement.cpp

    ../Engraver.cpp
    ../Renderer.cpp
    ../MusicStyle.cpp
)

add_executable(musicwidgettest ${musicwidgettest_SRCS})
ecm_mark_as_test(musicwidgettest)
target_link_libraries(musicwidgettest Qt6::Widgets Qt6::Test Qt6::Core)
