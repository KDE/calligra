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

import QtQuick 1.0
import CalligraActive 1.0

Item {
    id: docRootRect
    signal documentLoaded
    property alias loadProgress: canvas.loadProgress
    clip: true

    function openDocument(path) {
        canvas.openDocument(path);
    }

    function initToolbar() {
        if (canvas.documentType == CADocumentInfo.Spreadsheet) {
            toolbarLoader.source = "SpreadsheetToolbar.qml";
        } else if (canvas.documentType == CADocumentInfo.TextDocument) {
            toolbarLoader.source = "WordsToolbar.qml";
        } else if (canvas.documentType == CADocumentInfo.Presentation) {
            toolbarLoader.source = "PresentationToolbar.qml";
        }
    }

//     function toggleEdit() {
//         if (docFlickable.visible) {
//             docFlickable.visible = false;
//             canvas.z = 1
//         } else {
//             docFlickable.visible = true;
//             canvas.z = -1
//         }
//     }

    CanvasController {
        id: canvas

        anchors.fill: parent
        z: -1

        cameraX: docFlickable.contentX
        cameraY: docFlickable.contentY

        Component.onCompleted: documentLoaded.connect(initToolbar)
        onDocumentLoaded: docRootRect.documentLoaded()

        //searchString: findToolbar.searchString
    }

//     Button {
//         id: editModeButton
//         drawBackground: false
//         imageSource: "qrc:///images/document-edit.png"
//         anchors.left: parent.left
//         anchors.bottom: parent.bottom
//         height: 64
//         width: 64
//         z: 30
// 
//         onClicked: toggleEdit();
//     }
// 
//     FindToolbar {
//         id: findToolbar
//         height: 32
//
//         anchors.left: parent.left
//         anchors.right: parent.right
//         anchors.bottom: parent.bottom
//     }

    MouseArea {
        id: flickableMouseArea
        anchors.fill: parent
        drag.filterChildren: true

        Flickable {
            id: docFlickable
            x: canvas.x; y: canvas.y; width: canvas.width; height: canvas.height;

            contentWidth: canvas.docWidth; contentHeight: canvas.docHeight;
        }

        Loader {
            id: toolbarLoader
            property bool containsMouse: false

            anchors.fill: parent
            opacity: 0
        }

        Connections {
            target: toolbarLoader.item
            onContainsMouseChanged: toolbarLoader.containsMouse = toolbarLoader.item.containsMouse
        }
    }

    states : [
        State {
            name: "toolbarShown";
            when: (flickableMouseArea.pressed || toolbarLoader.containsMouse) && !docFlickable.moving
            PropertyChanges { target: toolbarLoader; opacity: 1 }
        }
    ]

    transitions : [
        Transition {
            from: "toolbarShown"
            SequentialAnimation {
                PauseAnimation { duration: 2000 }
                NumberAnimation {
                    target: toolbarLoader; properties: "opacity"; duration: 3000
                }
            }
        }
    ]
}
