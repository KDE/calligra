project(KexiMobile)

cmake_minimum_required(VERSION 2.8.0)

#cmake_policy CMP0017 was introduced in version 2.8.4
if(${CMAKE_VERSION} VERSION_GREATER 2.8.3)
    cmake_policy(SET CMP0017 NEW)
endif()

find_package(Qt4 REQUIRED)

include_directories( ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/kexi/widget
)

set(KexiMobile_SRCS main.cpp
    KexiMobileMainWindow.cpp
    KexiMobileWidget.cpp
    KexiMobileNavigator.cpp
    KexiMobileToolbar.cpp
)

qt4_automoc(${KexiMobile_SRCS})

add_executable(keximobile ${KexiMobile_SRCS})

target_link_libraries(keximobile Qt5::Core Qt5::Gui
    kexiextendedwidgets
    kexicore
    kdeui

    KDb
)

install(TARGETS keximobile ${INSTALL_TARGETS_DEFAULT_ARGS})
