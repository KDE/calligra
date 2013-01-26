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

Item {
    id: root
    property QtObject documentController
    property Item docRootItem
    property int animationDuration: 200
    property int commonHeight: 48
    property int commonWidth: 48

    Loader {
        id: topToolbarLoader
        height: root.commonHeight
        z: 1

        anchors.left: leftToolbarLoader.right
        anchors.right: rightToolbarLoader.left
        anchors.bottom: parent.top

        onSourceChanged: {
            if (source) {
                item.documentController = root.documentController
                item.docRootItem = root.docRootItem
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
            AnchorAnimation { duration: root.animationDuration }
        }
    }

    Loader {
        id: rightToolbarLoader
        width: root.commonWidth
        z: 1

        anchors.left: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        onSourceChanged: {
            if (source) {
                item.documentController = root.documentController
                item.docRootItem = root.docRootItem
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
            AnchorAnimation { duration: root.animationDuration }
        }
    }

    Loader {
        id: bottomToolbarLoader
        height: root.commonHeight
        z: 1

        anchors.left: leftToolbarLoader.right
        anchors.right: rightToolbarLoader.left
        anchors.top: parent.bottom

        onSourceChanged: {
            if (source) {
                item.documentController = root.documentController
                item.docRootItem = root.docRootItem
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
            AnchorAnimation { duration: root.animationDuration }
        }
    }

    Loader {
        id: leftToolbarLoader
        width: root.commonWidth
        z: 1

        anchors.right: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        onSourceChanged: {
            if (source) {
                item.documentController = root.documentController
                item.docRootItem = root.docRootItem
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
            AnchorAnimation { duration: root.animationDuration }
        }
    }

    Loader {
        id: centerOverlayLoader
        z: 1

        anchors.top: parent.verticalCenter
        anchors.right: parent.horizontalCenter
        anchors.bottom: parent.verticalCenter
        anchors.left: parent.horizontalCenter

        onSourceChanged: {
            if (source) {
                item.documentController = root.documentController
                item.docRootItem = root.docRootItem
            }
        }

        states: State {
            name: "shown"

            AnchorChanges {
                target: centerOverlayLoader
                anchors.top: topToolbarLoader.bottom
                anchors.right: rightToolbarLoader.left
                anchors.bottom: bottomToolbarLoader.top
                anchors.left: leftToolbarLoader.right
            }
        }

        transitions: Transition {
            AnchorAnimation { duration: root.animationDuration }
        }

        function toggle() { state = (state == "shown" ? "" : "shown") }
    }

    states: [
        State {
            name: "shown"
            PropertyChanges { target: topToolbarLoader; state: "shown" }
            PropertyChanges { target: rightToolbarLoader; state: "shown" }
            PropertyChanges { target: bottomToolbarLoader; state: "shown" }
            PropertyChanges { target: leftToolbarLoader; state: "shown" }
        },
        State {
            name: "hidden"
            PropertyChanges { target: topToolbarLoader; state: "" }
            PropertyChanges { target: rightToolbarLoader; state: "" }
            PropertyChanges { target: bottomToolbarLoader; state: "" }
            PropertyChanges { target: leftToolbarLoader; state: "" }
        }
    ]

    function toggle() { state = (state == "shown" ? "hidden" : "shown") }
    function toggleOverlay() { centerOverlayLoader.toggle() }

    function initToolbars() {
        topToolbarLoader.source = root.documentController.documentHandler.topToolbarSource
        rightToolbarLoader.source = root.documentController.documentHandler.rightToolbarSource
        bottomToolbarLoader.source = root.documentController.documentHandler.bottomToolbarSource
        leftToolbarLoader.source = root.documentController.documentHandler.leftToolbarSource
        centerOverlayLoader.source = root.documentController.documentHandler.centerOverlaySource
    }
}