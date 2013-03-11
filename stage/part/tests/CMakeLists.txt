set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories(${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${KOTEXT_INCLUDES} ${KDE4_INCLUDES}
        ${CMAKE_SOURCE_DIR}/libs/flake/tests
        ${CMAKE_BINARY_DIR}/libs/main
        ${CMAKE_SOURCE_DIR}/libs/kotext/tests
        ${CMAKE_SOURCE_DIR}/stage/part
)

########### next target ###############

set(TestShapeAnimations_test_SRCS TestShapeAnimations.cpp ModelTest.cpp)

kde4_add_unit_test(TestShapeAnimations TESTNAME stage-part-TestShapeAnimations  ${TestShapeAnimations_test_SRCS})

target_link_libraries(TestShapeAnimations calligrastageprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestCustomSlideShows_test_SRCS TestCustomSlideShows.cpp )

kde4_add_unit_test(TestCustomSlideShows TESTNAME stage-part-TestCustomSlideShows  ${TestCustomSlideShows_test_SRCS})

target_link_libraries(TestCustomSlideShows calligrastageprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestEditCustomSlideShowsCommand_test_SRCS TestEditCustomSlideShowsCommand.cpp )

kde4_add_unit_test(TestEditCustomSlideShowsCommand TESTNAME stage-part-TestEditCustomSlideShowsCommand  ${TestEditCustomSlideShowsCommand_test_SRCS})

target_link_libraries(TestEditCustomSlideShowsCommand calligrastageprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestAddCustomSlideShowCommand_test_SRCS TestAddCustomSlideShowCommand.cpp )

kde4_add_unit_test(TestAddCustomSlideShowCommand TESTNAME stage-part-TestAddCustomSlideShowCommand  ${TestAddCustomSlideShowCommand_test_SRCS})

target_link_libraries(TestAddCustomSlideShowCommand calligrastageprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestDelCustomSlideShowCommand_test_SRCS TestDelCustomSlideShowCommand.cpp )

kde4_add_unit_test(TestDelCustomSlideShowCommand TESTNAME stage-part-TestDelCustomSlideShowCommand  ${TestDelCustomSlideShowCommand_test_SRCS})

target_link_libraries(TestDelCustomSlideShowCommand calligrastageprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestRenameCustomSlideShowCommand_test_SRCS TestRenameCustomSlideShowCommand.cpp )

kde4_add_unit_test(TestRenameCustomSlideShowCommand TESTNAME stage-part-TestRenameCustomSlideShowCommand  ${TestRenameCustomSlideShowCommand_test_SRCS})

target_link_libraries(TestRenameCustomSlideShowCommand calligrastageprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestDeleteSlidesCommand_test_SRCS TestDeleteSlidesCommand.cpp )

kde4_add_unit_test(TestDeleteSlidesCommand TESTNAME stage-part-TestDeleteSlidesCommand  ${TestDeleteSlidesCommand_test_SRCS})

target_link_libraries(TestDeleteSlidesCommand calligrastageprivate ${QT_QTTEST_LIBRARY})
