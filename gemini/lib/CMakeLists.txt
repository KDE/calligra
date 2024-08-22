project(gemini)

set(gemini_SRCS
    GeminiMainWindow.cpp
)

add_library(gemini SHARED ${gemini_SRCS})
generate_export_header(gemini)
target_link_libraries(gemini
    Qt6::Quick
    Qt6::Gui
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(gemini PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)

install(TARGETS gemini ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
