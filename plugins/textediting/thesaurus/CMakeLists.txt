add_definitions(-DTRANSLATION_DOMAIN=\"calligra_textediting_thesaurus\")

include_directories( ${KOWIDGETS_INCLUDES} ${KOTEXT_INCLUDES} )

########### next target ###############

set(thesaurustool_SRCS
    ThesaurusDebug.cpp
    Thesaurus.cpp
    ThesaurusPlugin.cpp
    ThesaurusFactory.cpp
)


add_library(calligra_textediting_thesaurus MODULE ${thesaurustool_SRCS})

target_link_libraries(calligra_textediting_thesaurus
    kotext
    kowidgets
)

install(TARGETS calligra_textediting_thesaurus  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/textediting)


########### install files ###############

install( FILES  thesaurus.txt  DESTINATION ${KDE_INSTALL_DATADIR}/calligra/thesaurus)

