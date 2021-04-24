/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0

Rectangle {
    id: base;
    function show(message) {
        banner_text.text = message;
        opacity = 1;
        closeTimer.start();
    }

    anchors.fill: parent;
    anchors.margins: 20;
    color: "transparent";
    opacity: 0;
    Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
    z:42

    Rectangle {
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            margins: 20;
        }
        height: banner_text.paintedHeight + 20;
        color: Qt.rgba(128,128,128,0.9);
        radius: 20;
        border {
            width: 1;
            color: "silver";
        }
        clip: true;
    }
    Text {
        id: banner_text;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            margins: 30;
        }
        height: paintedHeight;
        font: Settings.theme.font("application");
        wrapMode: Text.Wrap;
    }
    Timer {
        id: closeTimer;
        interval: 3000;
        running: false;
        repeat: false;
        onTriggered: {
            base.opacity = 0;
        }
    }
}
