set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories(${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${KOTEXT_INCLUDES} 
        ${CMAKE_SOURCE_DIR}/libs/flake/tests
        ${CMAKE_BINARY_DIR}/libs/main
        ${CMAKE_SOURCE_DIR}/libs/kotext/tests
        ${CMAKE_SOURCE_DIR}/stage/part
)

########### next target ###############

set(TestShapeAnimations_test_SRCS TestShapeAnimations.cpp ModelTest.cpp)

add_executable(TestShapeAnimationsNAME stage-part-TestShapeAnimations  ${TestShapeAnimations_test_SRCS})
add_test(TestShapeAnimations TestShapeAnimations)
ecm_mark_as_test(TestShapeAnimations)

target_link_libraries(TestShapeAnimations calligrastageprivate Qt5::Test)

########### next target ###############

set(TestCustomSlideShows_test_SRCS TestCustomSlideShows.cpp )

add_executable(TestCustomSlideShowsNAME stage-part-TestCustomSlideShows  ${TestCustomSlideShows_test_SRCS})
add_test(TestCustomSlideShows TestCustomSlideShows)
ecm_mark_as_test(TestCustomSlideShows)

target_link_libraries(TestCustomSlideShows calligrastageprivate Qt5::Test)

########### next target ###############

set(TestEditCustomSlideShowsCommand_test_SRCS TestEditCustomSlideShowsCommand.cpp )

add_executable(TestEditCustomSlideShowsCommandNAME stage-part-TestEditCustomSlideShowsCommand  ${TestEditCustomSlideShowsCommand_test_SRCS})
add_test(TestEditCustomSlideShowsCommand TestEditCustomSlideShowsCommand)
ecm_mark_as_test(TestEditCustomSlideShowsCommand)

target_link_libraries(TestEditCustomSlideShowsCommand calligrastageprivate Qt5::Test)

########### next target ###############

set(TestAddCustomSlideShowCommand_test_SRCS TestAddCustomSlideShowCommand.cpp )

add_executable(TestAddCustomSlideShowCommandNAME stage-part-TestAddCustomSlideShowCommand  ${TestAddCustomSlideShowCommand_test_SRCS})
add_test(TestAddCustomSlideShowCommand TestAddCustomSlideShowCommand)
ecm_mark_as_test(TestAddCustomSlideShowCommand)

target_link_libraries(TestAddCustomSlideShowCommand calligrastageprivate Qt5::Test)

########### next target ###############

set(TestDelCustomSlideShowCommand_test_SRCS TestDelCustomSlideShowCommand.cpp )

add_executable(TestDelCustomSlideShowCommandNAME stage-part-TestDelCustomSlideShowCommand  ${TestDelCustomSlideShowCommand_test_SRCS})
add_test(TestDelCustomSlideShowCommand TestDelCustomSlideShowCommand)
ecm_mark_as_test(TestDelCustomSlideShowCommand)

target_link_libraries(TestDelCustomSlideShowCommand calligrastageprivate Qt5::Test)

########### next target ###############

set(TestRenameCustomSlideShowCommand_test_SRCS TestRenameCustomSlideShowCommand.cpp )

add_executable(TestRenameCustomSlideShowCommandNAME stage-part-TestRenameCustomSlideShowCommand  ${TestRenameCustomSlideShowCommand_test_SRCS})
add_test(TestRenameCustomSlideShowCommand TestRenameCustomSlideShowCommand)
ecm_mark_as_test(TestRenameCustomSlideShowCommand)

target_link_libraries(TestRenameCustomSlideShowCommand calligrastageprivate Qt5::Test)

########### next target ###############

set(TestDeleteSlidesCommand_test_SRCS TestDeleteSlidesCommand.cpp )

add_executable(TestDeleteSlidesCommandNAME stage-part-TestDeleteSlidesCommand  ${TestDeleteSlidesCommand_test_SRCS})
add_test(TestDeleteSlidesCommand TestDeleteSlidesCommand)
ecm_mark_as_test(TestDeleteSlidesCommand)

target_link_libraries(TestDeleteSlidesCommand calligrastageprivate Qt5::Test)
