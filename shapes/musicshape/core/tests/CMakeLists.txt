include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. ${KOFFICEUI_INCLUDES} )

#   macro(MUSICCORE_BUILD_TESTS)
#       foreach(_testname ${ARGN} ${musiccore_SRCS})
#           kde4_automoc(${_testname}.cpp)
#           kde4_add_test(${_testname} ${_testname}.cpp)
#           target_link_libraries(${_testname} ${QT_QTTEST_LIBRARY})
#           add_test(${_testname} ${_testname})
#       endforeach(_testname)
#   endmacro(MUSICCORE_BUILD_TESTS)
#
#   MUSICCORE_BUILD_TESTS(
#       SheetTest
#       VoiceBarTest
#   )

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
)

kde4_automoc(${SheetTest_test_SRCS})
kde4_add_test(SheetTest ${SheetTest_test_SRCS})
target_link_libraries(SheetTest  ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
add_test(shapes-musicshape-SheetTest ${EXECUTABLE_OUTPUT_PATH}/SheetTest)


########### next target ###############

set(VoiceBarTest_test_SRCS
    VoiceBarTest.cpp
    ../VoiceBar.cpp
    ../MusicElement.cpp
)

kde4_automoc(${VoiceBarTest_test_SRCS})
kde4_add_test(VoiceBarTest ${VoiceBarTest_test_SRCS})
target_link_libraries(VoiceBarTest  ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
add_test(shapes-musicshape-VoiceBarTest ${EXECUTABLE_OUTPUT_PATH}/VoiceBarTest)


