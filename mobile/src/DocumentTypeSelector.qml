/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <jhahoneyk@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

import QtQuick 1.0 as QML

QML.Rectangle {
    id: container
    
    property int buttonWidth
    property int buttonHeight
    property bool hidden : false
    
    width: parent.width; height: parent.height;

    QML.Row {
        id: docChoicesRow
        anchors.centerIn: parent
        spacing: 10

        Button {
            id: button1
            
            imageSource: "qrc:///images/words.png"
            text: "Document"
            width: buttonWidth; height: buttonHeight
        }
        Button {
            id: button2

            imageSource: "qrc:///images/tables.png"
            text: "Spreadsheet"
            width: buttonWidth; height: buttonHeight
        }
        Button {
            id: button3
            
            imageSource: "qrc:///images/stage.png"
            text: "Presentation"
            width: buttonWidth; height: buttonHeight

            QML.MouseArea {
                anchors.fill: parent
                onPressed: rootRect.state = "presentation"
            }
        }
    }
}
