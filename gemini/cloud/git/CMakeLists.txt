include_directories(${LIBGIT2_INCLUDE_DIR} ${LIBQGIT2_INCLUDE_DIR})

# LibQGit2 requires exception support to be turned on
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

set(git_plugin_SRCS
    gitplugin.cpp
    gitcontroller.cpp
    gitlogmodel.cpp
    documentlistmodel.cpp
    checkoutcreator.cpp
)

automoc4_add_library(calligrageminigitplugin MODULE ${git_plugin_SRCS})

target_link_libraries(calligrageminigitplugin
    Qt5::Declarative
    Qt5::Gui
    Qt5::Core
    ${LIBGIT2_LIBRARIES}
    ${LIBQGIT2_LIBRARIES}
    komain
)

install(TARGETS calligrageminigitplugin DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/Calligra/Gemini/Git)
install(FILES   qmldir                  DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/Calligra/Gemini/Git)
