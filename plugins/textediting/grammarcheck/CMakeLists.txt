project(grammarcheck)

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOTEXT_INCLUDES} ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} )

set(grammarcheck_SRCS
    GrammarCheckPlugin.cpp
    GrammarCheck.cpp
    GrammarCheckFactory.cpp
    BgGrammarCheck.cpp
    GrammarCheckMenu.cpp
    LinkGrammar.cpp
)

kde4_add_plugin(grammarcheck ${grammarcheck_SRCS})

target_link_libraries(grammarcheck kotext textlayout ${LINK_GRAMMAR_LIBRARIES})

install(TARGETS grammarcheck DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES grammarcheck.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
#add_subdirectory( tests )

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )
