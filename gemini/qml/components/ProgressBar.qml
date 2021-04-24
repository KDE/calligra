/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import org.calligra 1.0

Item {
    id: base;
    property int progress: 0;
    Rectangle {
        id: progressBase;
        anchors.centerIn: parent;
        height: Constants.LargeFontSize + 4;
        width: 208;
        radius: height / 2;
        border {
            width: 1;
            color: Settings.theme.color("components/dialog/progress/border");
        }
        color: Settings.theme.color("components/dialog/progress/background");
        Rectangle {
            id: progressBar;
            anchors {
                top: parent.top;
                left: parent.left;
                margins: 4;
            }
            radius: height / 2;
            width: progress >= 0 ? (progress * 2) + 1: 100;
            height: parent.height - 7;
            Behavior on width { PropertyAnimation { duration: 100; } }
            color: Settings.theme.color("components/dialog/progress/bar");
        }
    }
}
