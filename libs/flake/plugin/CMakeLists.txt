# SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: LGPL-2.0-or-later

add_library(flakeplugin SHARED FlakePlugin.cpp)
target_link_libraries(flakeplugin
    Qt5::Core
    Qt5::Qml
    flake
)

install(TARGETS flakeplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/calligra/flake)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/calligra/flake)


