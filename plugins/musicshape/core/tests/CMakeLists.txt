set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/..  )

# call: musicshape_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(MUSICSHAPE_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "shapes-music-"
    )
endmacro()

########### next target ###############

set(SheetTest_test_SRCS
    SheetTest.cpp
    ../Global.cpp
    ../Part.cpp
    ../Sheet.cpp
    ../PartGroup.cpp
    ../Bar.cpp
    ../Voice.cpp
    ../Staff.cpp
    ../VoiceBar.cpp
    ../StaffElement.cpp
    ../Clef.cpp
    ../StaffSystem.cpp
    ../VoiceElement.cpp
    ../KeySignature.cpp
    ../TimeSignature.cpp
    ../Note.cpp
    ../Chord.cpp
)

musicshape_add_unit_test(SheetTest
    ${SheetTest_test_SRCS}
    LINK_LIBRARIES Qt5::Test
)


########### next target ###############

set(VoiceBarTest_test_SRCS
    VoiceBarTest.cpp
    ../Global.cpp
    ../VoiceBar.cpp
    ../VoiceElement.cpp
    ../Sheet.cpp
    ../Bar.cpp
    ../PartGroup.cpp
    ../Part.cpp
    ../StaffSystem.cpp
    ../Staff.cpp
    ../StaffElement.cpp
    ../Voice.cpp
    ../Clef.cpp
    ../KeySignature.cpp
    ../TimeSignature.cpp
    ../Note.cpp
    ../Chord.cpp
)

musicshape_add_unit_test(VoiceBarTest
    ${VoiceBarTest_test_SRCS}
    LINK_LIBRARIES Qt5::Test
)

########### next target ###############

set(MusicXmlWriterTest_test_SRCS
    MusicXmlWriterTest.cpp
    ../Global.cpp
    ../MusicXmlWriter.cpp
    ../Part.cpp
    ../Sheet.cpp
    ../PartGroup.cpp
    ../Bar.cpp
    ../Voice.cpp
    ../Staff.cpp
    ../VoiceBar.cpp
    ../StaffElement.cpp
    ../Clef.cpp
    ../StaffSystem.cpp
    ../VoiceElement.cpp
    ../Chord.cpp
    ../Note.cpp
    ../KeySignature.cpp
    ../TimeSignature.cpp
)

musicshape_add_unit_test(MusicXmlWriterTest
    ${MusicXmlWriterTest_test_SRCS}
    LINK_LIBRARIES koodf Qt5::Test
)
