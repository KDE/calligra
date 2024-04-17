project(changecase)

add_definitions(-DTRANSLATION_DOMAIN=\"calligra_textediting_changecase\")

include_directories( ${KOWIDGETS_INCLUDES} ${KOTEXT_INCLUDES} )

set(changecase_SRCS
    ChangecaseDebug.cpp
    ChangecasePlugin.cpp
    Changecase.cpp
    ChangecaseFactory.cpp
)

add_library(calligra_textediting_changecase MODULE ${changecase_SRCS})

target_link_libraries(calligra_textediting_changecase
    kotext
    kowidgets
)

install(TARGETS calligra_textediting_changecase DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/textediting)
