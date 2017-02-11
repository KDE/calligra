include_directories(${LIBGIT2_INCLUDE_DIR})

set(git_plugin_SRCS
    gitplugin.cpp
    gitcontroller.cpp
    gitlogmodel.cpp
    documentlistmodel.cpp
    checkoutcreator.cpp
)

add_library(calligrageminigitplugin SHARED ${git_plugin_SRCS})

target_link_libraries(calligrageminigitplugin
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    ${LIBGIT2_LIBRARIES}
    komain
    KF5::I18n
)

install(TARGETS calligrageminigitplugin DESTINATION ${QML_INSTALL_DIR}/Calligra/Gemini/Git)
install(FILES   qmldir                  DESTINATION ${QML_INSTALL_DIR}/Calligra/Gemini/Git)
