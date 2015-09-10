/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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
