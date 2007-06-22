include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. ${KOGUIUTILS_INCLUDES} )

add_definitions( -DKDESRCDIR=\\"${CMAKE_CURRENT_SOURCE_DIR}\\" )
########### next target ###############

set(SheetTest_test_SRCS
    SheetTest.cpp
    ../Part.cpp
    ../Sheet.cpp
    ../PartGroup.cpp
    ../Bar.cpp
    ../Voice.cpp
    ../Staff.cpp
    ../VoiceBar.cpp
    ../StaffElement.cpp
    ../Clef.cpp
)

kde4_automoc(${SheetTest_test_SRCS})
kde4_add_unit_test(SheetTest TESTNAME shapes-musicshape-SheetTest ${SheetTest_test_SRCS})
target_link_libraries(SheetTest  ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})


########### next target ###############

set(VoiceBarTest_test_SRCS
    VoiceBarTest.cpp
    ../VoiceBar.cpp
    ../VoiceElement.cpp
)

kde4_automoc(${VoiceBarTest_test_SRCS})
kde4_add_unit_test(VoiceBarTest TESTNAME shapes-musicshape-VoiceBarTest ${VoiceBarTest_test_SRCS})
target_link_libraries(VoiceBarTest  ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})

########### next target ###############

set(MusicXmlWriterTest_test_SRCS
    MusicXmlWriterTest.cpp
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
)

kde4_automoc(${MusicXmlWriterTest_test_SRCS})
kde4_add_unit_test(MusicXmlWriterTest TESTNAME shapes-musicshape-MusicXmlWriterTest ${MusicXmlWriterTest_test_SRCS})
target_link_libraries(MusicXmlWriterTest  ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY} kstore)



