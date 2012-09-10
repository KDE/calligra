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

import QtQuick 1.0

Item {
    id: buttonContainer

    property alias text: buttonText.text
    property alias imageSource: buttonImage.source
    property alias textPosition: buttonContainer.state
    property bool containsMouse: mouseArea1.containsMouse || mouseArea2.containsMouse
    property bool drawBackground: true
    signal clicked
    signal entered
    signal exited

    Component {
        id: backgroundRect
        Rectangle {
            anchors.fill: parent
            radius: 5
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#FFFFFF" }
                GradientStop { position: 1.0; color: "#ABABAB" }
            }
            opacity: 0.5
        }
    }

    Loader {
        id: backgroundLoader

        anchors.fill: parent
        sourceComponent: drawBackground ? backgroundRect : undefined
    }

    Image {
        id: buttonImage

        anchors.horizontalCenter: buttonContainer.horizontalCenter
        width: height
        height: buttonContainer.height - buttonText.height
        smooth: true

        MouseArea {
            id: mouseArea1
            anchors.fill: parent
            hoverEnabled: true
            onClicked: buttonContainer.clicked()
            onEntered: buttonContainer.entered()
            onExited: buttonContainer.exited()
        }
    }

    Text {
        id: buttonText

        width: buttonContainer.width
        anchors.top: buttonImage.bottom;
        anchors.horizontalCenter: buttonImage.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        elide: Text.ElideMiddle

        MouseArea {
            id: mouseArea2
            anchors.fill: parent
            hoverEnabled: true
            onClicked: buttonContainer.clicked()
            onEntered: buttonContainer.entered()
            onExited: buttonContainer.exited()
        }
    }

    states: [
        State {
            name: "right"
            AnchorChanges {
                target: buttonImage
                anchors.verticalCenter: buttonContainer.verticalCenter
                anchors.horizontalCenter: undefined
                anchors.left: buttonContainer.left
            }
            PropertyChanges {
                target: buttonImage
                height: buttonContainer.height
            }
            AnchorChanges {
                target: buttonText
                anchors.verticalCenter: buttonImage.verticalCenter
                anchors.horizontalCenter: undefined
                anchors.left: buttonImage.right
            }
            PropertyChanges {
                target: buttonText
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                width: buttonContainer.width - buttonImage.width
            }
        }
    ]
}
