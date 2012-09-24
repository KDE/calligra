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

CanvasController {
    id: theCanvasController
    property alias documentController: docFlickable.docDocumentController
    signal needToolbars
    signal flickedToLeft
    signal flickedToRight

    caCanvasItem: canvasItem

    Flickable {
        id: docFlickable
        property bool alreadyNotified: false
        property QtObject docDocumentController
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.min(docFlickable.contentWidth, theCanvasController.width)
        height: Math.min(docFlickable.contentHeight, theCanvasController.height)
        contentWidth: canvasItem.width; contentHeight: canvasItem.height
        contentX: theCanvasController.cameraX
        contentY: theCanvasController.cameraY
        interactive: !canvasItem.editable

        CACanvasItem {
            id: canvasItem
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

        MouseArea {
            anchors.fill: parent
            visible: enabled
            enabled: !canvasItem.editable
            onClicked: theCanvasController.needToolbars()
        }

        onMovementEnded: alreadyNotified = false
        onContentXChanged: if (!alreadyNotified && moving && contentX > 200) {
            theCanvasController.flickedToLeft()
            alreadyNotified = true
        } else if (!alreadyNotified && moving && contentX < -200) {
            theCanvasController.flickedToRight()
            alreadyNotified = true
        }
    }

    Timer {
        id: flickableTimer; interval: 500
        onTriggered: docFlickable.returnToBounds()
    }

    function gotoNextPage()
    {
        if (docDocumentController.documentHandler) docDocumentController.documentHandler.gotoNextPage()
        flickableTimer.start()
    }
    function gotoPreviousPage()
    {
        if (docDocumentController.documentHandler) docDocumentController.documentHandler.gotoPreviousPage()
        flickableTimer.start()
    }

    function updateFlickableDirection() {
        if (docDocumentController.documentHandler) {
            var mode = docDocumentController.documentHandler.flickMode
            if (mode == CAAbstractDocumentHandler.FlickAutomatically)
                docFlickable.flickableDirection = Flickable.AutoFlickDirection
            else if (mode == CAAbstractDocumentHandler.FlickHorizontally)
                docFlickable.flickableDirection = Flickable.HorizontalFlick
            else if (mode == CAAbstractDocumentHandler.FlickVertically)
                docFlickable.flickableDirection = Flickable.VerticalFlick
            else if (mode == CAAbstractDocumentHandler.FlickBoth)
                docFlickable.flickableDirection = Flickable.HorizontalAndVerticalFlick
        }
    }

    function toggleEditing() {
        canvasItem.editable = !canvasItem.editable
    }
}
