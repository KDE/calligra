set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/kpresenter/part )

########### next target ###############

set(TestShapeAnimations_test_SRCS TestShapeAnimations.cpp )

kde4_add_unit_test(TestShapeAnimations TESTNAME kpresenter-part-TestShapeAnimations  ${TestShapeAnimations_test_SRCS})

target_link_libraries(TestShapeAnimations kpresenterprivate ${QT_QTTEST_LIBRARY})

