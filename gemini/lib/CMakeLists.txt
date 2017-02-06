project(gemini)

set(gemini_SRCS
    GeminiMainWindow.cpp
)

kde4_add_library(gemini SHARED ${gemini_SRCS})
target_link_libraries(gemini
    Qt5::Quick
    Qt5::Gui
    Qt5::Core
    Qt5::Widgets
)
