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

Rectangle {
    id: docRootRect
    signal documentLoaded

    clip: true
    color: "white"

    CADocumentController {
        id: docDocumentController
        canvasController: theCanvasController
        onDocumentOpened: {
            docRootRect.documentLoaded();
            docToolbars.initToolbars();
            theCanvasController.updateFlickableDirection()
        }
    }

    Image {
        id: previousPageImage
        anchors {
            top: parent.top; bottom: parent.bottom
            right: parent.left
        }
        source: docDocumentController.documentHandler.previousPageImage
        states: [
            State {
                name: "visible"
                AnchorChanges {
                    target: previousPageImage
                    anchors.right: undefined
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        ]
        transitions: Transition {
            from: ""
            SequentialAnimation {
                AnchorAnimation { duration: 500 }
                ScriptAction { script: theCanvasController.gotoPreviousPage() }
                ScriptAction { script: docRootRect.restoreCanvasControllerToCenter() }
            }
        }
    }

    CanvasContainer {
        id: theCanvasController
        anchors {
            top: parent.top; right: parent.right
            bottom: parent.bottom; left: parent.left;
        }

        documentController: docDocumentController

        onNeedToolbars: docToolbars.toggle()
        onFlickedToRight: { updateImages(); state = "movedToRight" }
        onFlickedToLeft: { updateImages(); state = "movedToLeft" }

        states: [
            State {
                name: "movedToLeft"
                AnchorChanges {
                    target: theCanvasController
                    anchors.left: undefined
                    anchors.right: parent.left
                }
                PropertyChanges { target: nextPageImage; state: "visible" }
            },
            State {
                name: "movedToRight"
                AnchorChanges {
                    target: theCanvasController
                    anchors.right: undefined
                    anchors.left: parent.right
                }
                PropertyChanges { target: previousPageImage; state: "visible" }
            }
        ]

        onStateChanged: if (state == "") {
            nextPageImage.state = ""
            previousPageImage.state = ""
        }

        transitions: Transition {
            from: ""
            AnchorAnimation { duration: 500 }
        }

        function updateImages()
        {
            previousPageImage.width = caCanvasItem.width
            nextPageImage.width = caCanvasItem.width
        }
    }

    Image {
        id: nextPageImage
        anchors {
            top: parent.top; bottom: parent.bottom
            left: parent.right
        }
        source: docDocumentController.documentHandler.nextPageImage
        states: [
            State {
                name: "visible"
                AnchorChanges {
                    target: nextPageImage
                    anchors.left: undefined
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        ]
        transitions: Transition {
            from: ""
            SequentialAnimation {
                AnchorAnimation { duration: 500 }
                ScriptAction { script: theCanvasController.gotoNextPage() }
                ScriptAction { script: docRootRect.restoreCanvasControllerToCenter() }
            }
        }
    }

    Toolbars {
        id: docToolbars
        anchors.fill: parent

        documentController: docDocumentController
        docRootItem: docRootRect
    }

    HomeScreen {
        id: homescreen
        anchors.fill: parent
    }

    onDocumentLoaded: homescreen.visible = false

    function openDocument(path) {
        docDocumentController.documentUri = path;
        docDocumentController.loadDocument();
    }

    function toggleEditing() {
        theCanvasController.toggleEditing()
    }

    function restoreCanvasControllerToCenter() {
        theCanvasController.state = ""
    }

    function hideOpenButton()
    {
        homescreen.hideOpenButton()
    }
}
