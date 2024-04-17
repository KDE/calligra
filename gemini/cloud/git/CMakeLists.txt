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

ecm_setup_version(${LIBGIT2_VERSION} VARIABLE_PREFIX GIT VERSION_HEADER libgit2_version.h)

target_link_libraries(calligrageminigitplugin
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    LibGit2::LibGit2
    komain
    KF6::I18n
)

install(TARGETS calligrageminigitplugin DESTINATION ${KDE_INSTALL_QMLDIR}/Calligra/Gemini/Git)
install(FILES   qmldir                  DESTINATION ${KDE_INSTALL_QMLDIR}/Calligra/Gemini/Git)
