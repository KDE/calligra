set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories(${KOTEXT_INCLUDES}
        ${CMAKE_SOURCE_DIR}/libs/flake/tests
        ${CMAKE_BINARY_DIR}/libs/main
        ${CMAKE_SOURCE_DIR}/libs/text/tests
        ${CMAKE_SOURCE_DIR}/words/part
)

add_definitions(-DFILES_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")
add_definitions(-DFILES_OUTPUT_DIR="${CMAKE_CURRENT_BINARY_DIR}/")

# call: words_part_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(WORDS_PART_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "words-part-"
    )
    set_tests_properties(words-part-${_TEST_NAME} PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_PLUGIN_PATH=${CMAKE_BINARY_DIR}/bin:${CMAKE_INSTALL_PREFIX}/lib64/plugins;XDG_DATA_DIRS=${CMAKE_INSTALL_PREFIX}/share:/usr/local/share:/usr/share;LSAN_OPTIONS=suppressions=${CMAKE_SOURCE_DIR}/sanitizers.supp")
endmacro()

# call: words_part_frame_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(WORDS_PART_FRAME_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "words-part-frame-"
    )
endmacro()

########### next target ###############

# words_part_add_unit_test(TestPageManager
#     TestPageManager.cpp
#     LINK_LIBRARIES wordsprivate Qt6::Test
# )

########### next target ###############

words_part_add_unit_test(TestClipToPage
    TestClipToPage.cpp
    LINK_LIBRARIES wordsprivate Qt6::Test
)

########### next target ###############

words_part_add_unit_test(TestTextStatistics
    TestTextStatistics.cpp
    LINK_LIBRARIES wordsprivate Qt6::Test
)

########### next target ###############

# words_part_add_unit_test(TestViewMode
#     TestViewMode.cpp
#     LINK_LIBRARIES wordsprivate Qt6::Test
# )

########### next target ###############

# words_part_add_unit_test(TestPageTextInfo
#     TestPageTextInfo.cpp
#     LINK_LIBRARIES wordsprivate Qt6::Test
# )

########### next target ###############

# words_part_frame_add_unit_test(TestFrameLayout
#     TestFrameLayout.cpp
#     LINK_LIBRARIES wordsprivate Qt6::Test
# )

########### next target ###############

# words_part_frame_add_unit_test(TestTextFrameSorting
#     TestTextFrameSorting.cpp
#     LINK_LIBRARIES wordsprivate Qt6::Test
# )

########### next target ###############

# words_part_frame_add_unit_test(TestTextFrameManagement
#     TestTextFrameManagement.cpp
#     LINK_LIBRARIES wordsprivate Qt6::Test
# )

########### next target ###############

if( SHOULD_BUILD_FEATURE_RDF )
# words_part_add_unit_test(TestRdf
#     TestRdf.cpp
#     LINK_LIBRARIES wordsprivate Qt6::Test
# )
endif()
