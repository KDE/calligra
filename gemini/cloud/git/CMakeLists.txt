find_package(Libgit2 REQUIRED)
include_directories(${LIBGIT2_INCLUDE_DIR})

set(git_plugin_SRCS
    gitplugin.cpp
    gitcontroller.cpp
    documentlistmodel.cpp
    checkoutcreator.cpp
)

automoc4_add_library(calligrageminigitplugin MODULE ${git_plugin_SRCS})

target_link_libraries(calligrageminigitplugin
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    kfile
    ${LIBGIT2_LIBRARIES}
    komain
)

install(TARGETS calligrageminigitplugin DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/Calligra/Gemini/Git)
install(FILES   qmldir                  DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/Calligra/Gemini/Git)
