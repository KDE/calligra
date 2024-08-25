# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clauses

add_executable(calligralauncher main.cpp helper.cpp helper.h)

qt_add_qml_module(calligralauncher
    URI org.kde.calligra
    QML_FILES
        Main.qml
    DEPENDENCIES
        QtQuick
        org.kde.kirigamiaddons.formcard
)

target_link_libraries(calligralauncher PUBLIC
    Qt::Quick
    Qt::QuickControls2
    Qt::Widgets
    Qt::Svg
    KF6::I18n
    KF6::CoreAddons
    KF6::Crash
    KF6::KIOWidgets
)

install(TARGETS calligralauncher ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install( PROGRAMS org.kde.calligra.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install( FILES org.kde.calligra.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
