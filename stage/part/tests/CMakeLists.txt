set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories(${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${KOTEXT_INCLUDES}
        ${CMAKE_SOURCE_DIR}/libs/flake/tests
        ${CMAKE_BINARY_DIR}/libs/main
        ${CMAKE_SOURCE_DIR}/libs/text/tests
        ${CMAKE_SOURCE_DIR}/stage/part
)

# call: stage_part_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(STAGE_PART_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "stage-part-"
    )
endmacro()

########### next target ###############

stage_part_add_unit_test(TestShapeAnimations
    TestShapeAnimations.cpp ModelTest.cpp
    LINK_LIBRARIES calligrastageprivate Qt6::Test
)

########### next target ###############

stage_part_add_unit_test(TestCustomSlideShows
    TestCustomSlideShows.cpp
    LINK_LIBRARIES calligrastageprivate Qt6::Test
)

########### next target ###############

stage_part_add_unit_test(TestEditCustomSlideShowsCommand
    TestEditCustomSlideShowsCommand.cpp
    LINK_LIBRARIES calligrastageprivate Qt6::Test
)

########### next target ###############

stage_part_add_unit_test(TestAddCustomSlideShowCommand
    TestAddCustomSlideShowCommand.cpp
    LINK_LIBRARIES calligrastageprivate Qt6::Test
)

########### next target ###############

stage_part_add_unit_test(TestDelCustomSlideShowCommand
    TestDelCustomSlideShowCommand.cpp
    LINK_LIBRARIES calligrastageprivate Qt6::Test
)

########### next target ###############

stage_part_add_unit_test(TestRenameCustomSlideShowCommand
    TestRenameCustomSlideShowCommand.cpp
    LINK_LIBRARIES calligrastageprivate Qt6::Test
)

########### next target ###############

stage_part_add_unit_test(TestDeleteSlidesCommand
    TestDeleteSlidesCommand.cpp
    LINK_LIBRARIES calligrastageprivate Qt6::Test
)
