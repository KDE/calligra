set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/libs/widgetutils )

# call: kowidgetutils_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(KOWIDGETUTILS_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "libs-kowidgetutils-"
    )
endmacro()

########### next target ###############

kowidgetutils_add_unit_test(KoPropertiesTest KoPropertiesTest.cpp  LINK_LIBRARIES kowidgetutils Qt6::Test)

########### next target ###############

set(kofiledialogtester_SRCS KoFileDialogTester.cpp)
qt_wrap_ui(kofiledialogtester_SRCS KoFileDialogTester.ui)
add_executable(kofiledialogtester ${kofiledialogtester_SRCS})
target_link_libraries(kofiledialogtester kowidgetutils)
