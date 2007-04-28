# Tests don't need to go into toplevel/bin, they are fine in the current dir.
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. ${KOFFICEUI_INCLUDES} )

macro(MUSICCORE_BUILD_TESTS)
    foreach(_testname ${ARGN})
        kde4_automoc(${_testname}.cpp)
        kde4_add_test(${_testname} ${_testname}.cpp)
        target_link_libraries(${_testname} musiccore ${QT_QTTEST_LIBRARY})
        add_test(${_testname} ${_testname})
    endforeach(_testname)
endmacro(MUSICCORE_BUILD_TESTS)

MUSICCORE_BUILD_TESTS(
    SheetTest
    VoiceBarTest
)
