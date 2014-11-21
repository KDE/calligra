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

import QtQuick 1.1

Rectangle {
    id: i_infobanner; anchors.fill: parent; color: "transparent"; visible: false; z:42
    property alias text: banner_text.text

    Rectangle {
        x: 20; y: 20; width: parent.width-40; height: banner_text.paintedHeight + 10;
        color: Qt.rgba(128,128,128,0.9);
        radius: 20; clip: true
        Text {
            id: banner_text
            //anchors.fill: parent
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 10
            anchors.topMargin: 10
            font: Settings.theme.font("application");
            wrapMode: Text.WordWrap
        }
    }
    onVisibleChanged: {
        //console.log("Infobanneri visible changes")
        if(visible == true) closeTimer.start();
    }
    Timer {
        id: closeTimer; interval: 3000; running: false; repeat: false; onTriggered: i_infobanner.visible = false
    }
}
