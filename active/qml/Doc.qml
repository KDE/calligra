/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
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

import QtQuick 1.1
import CalligraActive 1.0

Item {
    id: docRootRect
    signal documentLoaded

    clip: true

    CADocumentController {
        id: docDocumentController
        canvasController: theCanvasController
        onDocumentOpened: {
            docRootRect.documentLoaded();
            docToolbars.initToolbars();
        }
    }

    CanvasController {
        id: theCanvasController
        anchors.fill: parent
        caCanvasItem: canvasItem

        Flickable {
            id: docFlickable
            anchors.fill: parent
            contentWidth: canvasItem.width; contentHeight: canvasItem.height
            contentX: Math.max(theCanvasController.cameraX - width/2, 0)
            contentY: Math.max(theCanvasController.cameraY - height/2, 0)

            CACanvasItem {
                id: canvasItem
                editable: false
//                 width: docRootRect.width

                Rectangle {
                    color: "red"; opacity: 0.1; radius: 10; anchors.fill: parent; z: 2

                    MouseArea {
                        anchors.fill: parent
                        onClicked: docToolbars.toggle()
                    }
                }
            }

            Behavior on contentX {
                NumberAnimation {
                    duration: 1000
                    easing.type: Easing.OutExpo
                }
            }

            Behavior on contentY {
                NumberAnimation {
                    duration: 1000
                    easing.type: Easing.OutExpo
                }
            }
        }
    }

    Toolbars {
        id: docToolbars
        anchors.fill: parent

        documentController: docDocumentController
        docRootItem: docRootRect
    }

    function openDocument(path) {
        docDocumentController.documentUri = path;
        docDocumentController.loadDocument();
    }

    function toggleEditing() {
        docFlickable.visible = docFlickable.visible ? false : true
    }
}
