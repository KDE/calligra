project(spellcheck)

add_definitions(-DTRANSLATION_DOMAIN=\"calligra_textediting_spellcheck\")

if(BUILD_TESTING)
    add_subdirectory( tests )
endif()

include_directories(   ${TEXTLAYOUT_INCLUDES} ${FLAKE_INCLUDES} )

set(spellcheck_SRCS
    SpellCheckDebug.cpp
    SpellCheckPlugin.cpp
    SpellCheck.cpp
    SpellCheckFactory.cpp
    BgSpellCheck.cpp
    SpellCheckMenu.cpp
)

add_library(calligra_textediting_spellcheck MODULE ${spellcheck_SRCS})

target_link_libraries(calligra_textediting_spellcheck
    kotext
    kotextlayout
    KF6::SonnetCore
    KF6::SonnetUi
)

install(TARGETS calligra_textediting_spellcheck DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/textediting)
