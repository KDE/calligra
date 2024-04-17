project(autocorrection)

add_definitions(-DTRANSLATION_DOMAIN=\"calligra_textediting_autocorrect\")

add_subdirectory(libs)
add_subdirectory(data)

include_directories(${KOTEXT_INCLUDES} ${AUTOCORRECTION_INCLUDES})

set(autocorrect_SRCS
    AutocorrectDebug.cpp
    AutocorrectPlugin.cpp
    Autocorrect.cpp
    AutocorrectFactory.cpp
)

add_library(calligra_textediting_autocorrect MODULE ${autocorrect_SRCS})

target_link_libraries(calligra_textediting_autocorrect
    PRIVATE
    autocorrection
    kotext
    Qt::Core
    Qt::Xml
)

install(TARGETS calligra_textediting_autocorrect DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/textediting)
