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
import org.kde.plasma.components 0.1 as PlasmaComponents

Item
{
    id: root
    property QtObject documentController
    property Item docRootItem

    PlasmaComponents.ToolBar {
        anchors.centerIn: parent
        anchors.fill: parent

        tools: Row {
            anchors.centerIn: parent
            height: parent.height
            spacing: 10

            PlasmaComponents.ToolButton {
                id: slideshowDelayLabel
                height: parent.height
                width: height
                checkable: true

                iconSource: "player-time"
                text: Math.floor(slideshowDelaySlider.value.toString())

                PlasmaComponents.Slider {
                    id: slideshowDelaySlider
                    width: parent.height
                    height: parent.checked ? 256 : 0
                    visible: parent.checked
                    anchors.top: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter

                    orientation: Qt.Vertical
                    inverted: true
                    minimumValue: 1
                    maximumValue: 30
                    value: 5

                    onValueChanged: documentController.documentHandler.slideshowDelay = Math.floor(value)
                    onPressedChanged: if (!pressed) slideshowDelayLabel.checked = false
                }
            }

            PlasmaComponents.Label {
                id: currentNumber
                height: parent.height
                width: height

                text: i18n("Slide %1 of %2", root.documentController.documentHandler.currentSlideNumber, root.documentController.documentHandler.totalNumberOfSlides)
            }

            PlasmaComponents.ToolButton {
                id: startPresentationButton
                iconSource: "view-presentation"
                height: parent.height
                width: parent.height
                checkable: true

                onCheckedChanged: {
                    if (checked) {
                        root.documentController.documentHandler.startSlideshow()
                    } else {
                        root.documentController.documentHandler.stopSlideshow()
                    }
                }

                function slideshowStarted()
                {
                    root.parent.parent.state = "hidden"
                }

                function slideshowStopped()
                {
                    startPresentationButton.checked = false
                    root.parent.parent.state = "shown"
                }
            }

            PlasmaComponents.ToolButton {
                id: showOverlayButton
                iconSource: "stage"
                height: parent.height
                width: parent.height

                onClicked: root.parent.parent.toggleOverlay()
            }
        }
    }

    onDocumentControllerChanged: {
        root.documentController.documentHandler.slideshowStopped.connect(startPresentationButton.slideshowStopped)
        root.documentController.documentHandler.slideshowStarted.connect(startPresentationButton.slideshowStarted)
    }
}
