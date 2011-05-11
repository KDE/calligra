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
import CalligraMobile 1.0

Item {
    id: docRootRect
    signal documentLoaded
    clip: true

    function openDocument(path) {
        canvas.openDocument(path);
    }

    function initToolbar() {
        if (canvas.documentType == CanvasController.Spreadsheet) {
            toolbarLoader.source = "SpreadsheetToolbar.qml"
        } else if (canvas.documentType == CanvasController.TextDocument) {
            toolbarLoader.source = "WordsToolbar.qml"
        } else if (canvas.documentType == CanvasController.Presentation) {
            toolbarLoader.source = "PresentationToolbar.qml"
        }
    }

    Loader {
        id: toolbarLoader

        height: parent.height
        width: parent.width*0.05

        anchors.left: parent.left; anchors.top: parent.top;
    }

    CanvasController {
        id: canvas

        height: parent.height
        width: parent.width*0.95
        anchors.left: toolbarLoader.right; anchors.top: parent.top;

        cameraX: docFlickable.contentX
        cameraY: docFlickable.contentY

        Component.onCompleted: documentLoaded.connect(initToolbar)
        onDocumentLoaded: docRootRect.documentLoaded()
    }

    Flickable {
        id: docFlickable
        x: canvas.x; y: canvas.y; width: canvas.width; height: canvas.height;

        contentWidth: proxyItem.width; contentHeight: proxyItem.height;
        clip: true

        Item {
            id: proxyItem
            height: canvas.docHeight
            width: canvas.docWidth
        }
    }

    //Component.onCompleted: canvas.openDocument(fileName);
}
