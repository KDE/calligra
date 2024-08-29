// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.components as Components

Controls.ApplicationWindow {
    id: root

    visible: true
    width: contentRow.implicitWidth + Kirigami.Units.largeSpacing * 4
    height: contentRow.implicitHeight + Kirigami.Units.largeSpacing * 4

    background: Controls.Control {
        background: Components.DialogRoundedBackground {}
        leftInset: Kirigami.Units.largeSpacing
        topInset: Kirigami.Units.largeSpacing
        rightInset: Kirigami.Units.largeSpacing
        bottomInset: Kirigami.Units.largeSpacing
    }

    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WA_TranslucentBackground | Qt.NoDropShadowWindowHint
    color: "transparent"

    Shortcut {
        sequence: StandardKey.Cancel
        onActivated: Qt.quit()
    }

    Helper {
        id: helper
    }

    RowLayout {
        id: contentRow

        x: Kirigami.Units.largeSpacing * 2
        y: Kirigami.Units.largeSpacing * 2

        spacing: Kirigami.Units.smallSpacing
        Repeater {
            model: [
                {
                    name: i18n("Calligra Words"),
                    icon: "calligrawords",
                    desktopId: "calligrawords"
                },
                {
                    name: i18n("Calligra Stage"),
                    icon: "calligrasheets",
                    desktopId: "calligrastage"
                },
                {
                    name: i18n("Calligra Sheets"),
                    icon: "calligrastage",
                    desktopId: "calligrasheets"
                },
                {
                    name: i18n("Karbon"),
                    icon: "karbon",
                    desktopId: "karbon"
                },
                {
                    name: i18n("Quit"),
                    icon: "application-exit-symbolic",
                    desktopId: ""
                }
            ]

            Delegates.RoundedItemDelegate {
                leftPadding: Kirigami.Units.largeSpacing
                topPadding: Kirigami.Units.largeSpacing
                rightPadding: Kirigami.Units.largeSpacing
                bottomPadding: Kirigami.Units.largeSpacing

                onClicked: helper.execute(modelData.desktopId)

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                contentItem: ColumnLayout {
                    Kirigami.Icon {
                        source: modelData.icon
                        Layout.preferredWidth: Kirigami.Units.iconSizes.huge * 2
                        Layout.preferredHeight: Kirigami.Units.iconSizes.huge * 2
                    }

                    Kirigami.Heading {
                        level: 2
                        text: modelData.name
                        Layout.fillWidth: true
                        horizontalAlignment: Qt.AlignHCenter
                    }
                }
            }
        }
    }
}
