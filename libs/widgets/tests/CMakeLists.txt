set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories(  ${KOMAIN_INCLUDES}  ${CMAKE_SOURCE_DIR}/libs/widgets)

add_definitions(-DFILES_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")
add_definitions(-DFILES_OUTPUT_DIR="${CMAKE_CURRENT_BINARY_DIR}")

find_package(KF6ThreadWeaver ${REQUIRED_KF6_VERSION} REQUIRED)

#add_subdirectory(filedialogtester)

# call: kowidgets_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(KOWIDGETS_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "libs-kowidgets-"
    )
endmacro()

########### next target ###############

kowidgets_add_unit_test(zoomhandler_test zoomhandler_test.cpp  LINK_LIBRARIES kowidgets Qt6::Test)

########### next target ###############

kowidgets_add_unit_test(zoomcontroller_test zoomcontroller_test.cpp  LINK_LIBRARIES kowidgets Qt6::Test)

########### next target ###############

kowidgets_add_unit_test(KoResourceTaggingTest KoResourceTaggingTest.cpp  LINK_LIBRARIES kowidgets Qt6::Test)

########### next target ###############

kowidgets_add_unit_test(KoProgressUpdaterTest KoProgressUpdater_test.cpp  LINK_LIBRARIES kowidgets KF6::ThreadWeaver Qt6::Test)

########### end ###############
