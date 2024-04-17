project(textvariables)

add_definitions(-DTRANSLATION_DOMAIN=\"calligra_textinlineobject_variables\")

include_directories(
    
    
    ${KOTEXT_INCLUDES}
    ${TEXTLAYOUT_INCLUDES}
)

set(textvariables_SRCS
    VariablesDebug.cpp
    VariablesPlugin.cpp
    UserVariable.cpp
    UserVariableFactory.cpp
    UserVariableOptionsWidget.cpp
    DateVariable.cpp
    DateVariableFactory.cpp
    FixedDateFormat.cpp
    PageVariable.cpp
    PageVariableFactory.cpp
    InfoVariable.cpp
    InfoVariableFactory.cpp
    ChapterVariable.cpp
    ChapterVariableFactory.cpp
)


ki18n_wrap_ui( textvariables_SRCS
    FixedDateFormat.ui
)

add_library(calligra_textinlineobject_variables MODULE ${textvariables_SRCS})

target_link_libraries(calligra_textinlineobject_variables
    kotext
    kotextlayout
)

install(TARGETS calligra_textinlineobject_variables DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/textinlineobjects)
