set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/plugins/textediting/spellcheck
    ${KOTEXT_INCLUDES})

########### next target ###############

set(TestSpellCheck_SRCS
    TestSpellCheck.cpp
    ../BgSpellCheck.cpp
    ../SpellCheckDebug.cpp
)

ecm_add_test( ${TestSpellCheck_SRCS}
    TEST_NAME "TestSpellCheck"
    NAME_PREFIX "textediting-spellcheck-"
    LINK_LIBRARIES kotext KF6::SonnetCore Qt6::Test
)
