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
    signal toggleToolbars
    clip: true

    CADocumentController {
        id: docDocumentController
        canvasController: canvas
        onDocumentOpened: {
            docRootRect.documentLoaded();
            docRootRect.initToolbar();
        }
    }

    CanvasController {
        id: canvas
        anchors.fill: parent

        cameraX: docFlickable.contentX
        cameraY: docFlickable.contentY

        Flickable {
            id: docFlickable
            anchors.fill: parent
            z: 1

            contentWidth: canvas.docWidth; contentHeight: canvas.docHeight;

            MouseArea {
                anchors.fill: parent
                z: 1

                onClicked: docRootRect.toggleToolbars()
            }
        }
    }

    Loader {
        id: topToolbarLoader
        height: 32
        z: 1

        anchors.left: leftToolbarLoader.right
        anchors.right: rightToolbarLoader.left
        anchors.bottom: parent.top

        onSourceChanged: {
            if (source) {
                item.documentController = docDocumentController
            }
        }

        states: State {
            name: "shown"

            AnchorChanges {
                target: topToolbarLoader
                anchors.bottom: undefined
                anchors.top: parent.top
            }
        }

        transitions: Transition {
            AnchorAnimation { duration: 300 }
        }

        function toggle() { state = state ? "" : "shown" }
    }

    Loader {
        id: rightToolbarLoader
        width: 32
        z: 1

        anchors.left: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        onSourceChanged: {
            if (source) {
                item.documentController = docDocumentController
                state = "shown"
            }
        }

        states: State {
            name: "shown"

            AnchorChanges {
                target: rightToolbarLoader
                anchors.left: undefined
                anchors.right: parent.right
            }
        }

        transitions: Transition {
            AnchorAnimation { duration: 300 }
        }

        function toggle() { state = state ? "" : "shown" }
    }

    Loader {
        id: bottomToolbarLoader
        height: 32
        z: 1

        anchors.left: leftToolbarLoader.right
        anchors.right: rightToolbarLoader.left
        anchors.top: parent.bottom

        onSourceChanged: {
            if (source) {
                item.documentController = docDocumentController
                state = "shown"
            }
        }

        states: State {
            name: "shown"

            AnchorChanges {
                target: bottomToolbarLoader
                anchors.top: undefined
                anchors.bottom: parent.bottom
            }
        }

        transitions: Transition {
            AnchorAnimation { duration: 300 }
        }

        function toggle() { state = state ? "" : "shown" }
    }

    Loader {
        id: leftToolbarLoader
        width: 32
        z: 1

        anchors.right: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        onSourceChanged: {
            if (source) {
                item.documentController = docDocumentController
                state = "shown"
            }
        }

        states: State {
            name: "shown"

            AnchorChanges {
                target: leftToolbarLoader
                anchors.right: undefined
                anchors.left: parent.left
            }
        }

        transitions: Transition {
            AnchorAnimation { duration: 300 }
        }

        function toggle() { state = state ? "" : "shown" }
    }

    onToggleToolbars : {
        topToolbarLoader.toggle()
        rightToolbarLoader.toggle()
        bottomToolbarLoader.toggle()
        leftToolbarLoader.toggle()
    }

    function initToolbar() {
        topToolbarLoader.source = docDocumentController.documentHandler().topToolbarSource
        rightToolbarLoader.source = docDocumentController.documentHandler().rightToolbarSource
        bottomToolbarLoader.source = docDocumentController.documentHandler().bottomToolbarSource
        leftToolbarLoader.source = docDocumentController.documentHandler().leftToolbarSource
    }

    function openDocument(path) {
        docDocumentController.documentUri = path;
        docDocumentController.loadDocument();
    }
}
