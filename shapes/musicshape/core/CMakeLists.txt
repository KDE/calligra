add_subdirectory( tests )

include_directories( ${KOFFICEUI_INCLUDES} )

set( musiccore_SRCS
    Sheet.cpp
    Part.cpp
    PartGroup.cpp
    Staff.cpp
    Voice.cpp
    Bar.cpp
    VoiceBar.cpp
    MusicElement.cpp
    Chord.cpp
    Note.cpp
)

kde4_add_library(musiccore ${musiccore_SRCS})

target_link_libraries(musiccore kofficeui)

