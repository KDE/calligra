/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

Item {
    id: base;
    height: messageStack.height;
    width: messageStack.width;
    function showMessage(message, iconName) {
        var block = messageBlock.createObject(messageStack);
        block.text = message;
    }

    Component {
        id: messageBlock;
        Item {
            id: message;
            property alias text: label.text;
            NumberAnimation on opacity {
                to: 0;
                duration: 2000;
                onRunningChanged: {
                    if(!running) {
                        message.destroy();
                    }
                }
            }
            height: Constants.GridHeight;
            width: Constants.GridWidth * 3;
            Rectangle {
                anchors.fill: parent;
                color: "gray";
                border {
                    width: 2;
                    color: "silver";
                }
                radius: height / 2;
                opacity: 0.5;
            }
            Text {
                id: label;
                anchors.centerIn: parent;
                font.family: "Source Sans Pro"
                color: "white";
                font.bold: true;
                font.pixelSize: Constants.LargeFontSize;
            }
        }
    }

    Column {
        id: messageStack;
        anchors {
            bottom: parent.bottom;
        }
        height: childrenRect.height;
    }
}
