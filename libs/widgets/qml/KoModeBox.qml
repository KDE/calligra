// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.calligra.flake 1.0

QQC2.Page {
    width: 400
    height: 50
    header: QQC2.TabBar {
        Repeater {
            model: KoToolManager.toolActionList
            
            QQC2.TabButton {
                icon.name: modelData.iconName
                text: modelData.iconText
            }
        }
    }
}
