include_directories(${LIBGIT2_INCLUDE_DIR})

set(git_plugin_SRCS
    gitplugin.cpp
    gitcontroller.cpp
    gitlogmodel.cpp
    documentlistmodel.cpp
    checkoutcreator.cpp
)

if(WIN32)
add_library(calligrageminigitplugin MODULE ${git_plugin_SRCS})
else()
add_library(calligrageminigitplugin SHARED ${git_plugin_SRCS})
endif()

target_link_libraries(calligrageminigitplugin
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    ${LIBGIT2_LIBRARIES}
    komain
    KF5::I18n
)

install(TARGETS calligrageminigitplugin DESTINATION ${KDE_INSTALL_QMLDIR}/Calligra/Gemini/Git)
install(FILES   qmldir                  DESTINATION ${KDE_INSTALL_QMLDIR}/Calligra/Gemini/Git)
