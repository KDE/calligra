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
    id: rootRect

    width: 800; height: 600;
    gradient: QML.Gradient {
         QML.GradientStop { position: 0.0; color: "#808080" }
         QML.GradientStop { position: 1.0; color: "#303030" }
    }

    QML.Row {
        id: docChoices
        
        anchors.centerIn: parent
        
        spacing: 10

        Button {
            id: button1
            
            imageSource: "qrc:///images/words.png"
            text: "Document"
            width: 100; height: 100
        }
        Button {
            id: button2
            
            imageSource: "qrc:///images/tables.png"
            text: "Spreadsheet"
            width: 100; height: 100
        }
        Button {
            id: button3
            
            imageSource: "qrc:///images/stage.png"
            text: "Presentation"
            width: 100; height: 100

            QML.MouseArea {
                anchors.fill: parent
                onPressed: rootRect.state = "presentation"
            }
        }
    }

    PresentationTemplatesView {
        id: presentationTemplatesView
        height: 400; width: 800;
        
        anchors.centerIn: parent
        visible: false
    }

    states: [
        QML.State {
            name: "presentation"
            QML.PropertyChanges { target: docChoices; visible: false }
            QML.PropertyChanges { target: presentationTemplatesView; visible: true }
        }
    ]
}
